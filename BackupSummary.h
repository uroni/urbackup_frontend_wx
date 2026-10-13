/*************************************************************************
*    UrBackup - Client/Server backup system
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

//Describes the interval and min/max number of backups settings in plain text
//and warns about combinations the server cannot keep. Same texts and rules as
//urbackupserver/www/js/backup_summary.js of the server web interface.
namespace backup_summary
{
	struct SInterval
	{
		enum EType
		{
			type_seconds,
			type_disabled,
			//Depends on the time of day or week
			type_varies,
			//No usable number
			type_none
		};

		SInterval() : type(type_none), seconds(0) {}
		SInterval(EType type, double seconds = 0) : type(type), seconds(seconds) {}

		EType type;
		double seconds;
	};

	struct SSettings
	{
		SSettings() : image(false), max_full(-1), min_full(-1), max_incr(-1), min_incr(-1) {}

		bool image;
		SInterval incr;
		SInterval full;
		//-1: not a number
		int max_full;
		int min_full;
		int max_incr;
		int min_incr;
		//The incremental image styles in use (true: based on the last image backup)
		std::vector<bool> to_last;
	};

	struct SSummary
	{
		std::vector<std::string> info;
		std::vector<std::string> warn;
	};

	//Returns the translation of an English text
	typedef std::string(*translate_t)(const std::string& text);

	typedef std::map<std::string, std::string> params_t;

	const int max_slots = 40000;
	const int max_backups = 4000;
	//Backups looked at while running the schedule, to keep the window responsive
	const double max_work = 20000000;

	inline std::string msg(translate_t translate, const std::string& text, const params_t& params = params_t())
	{
		std::string ret = translate != NULL ? translate(text) : text;
		for (params_t::const_iterator it = params.begin(); it != params.end(); ++it)
		{
			std::string key = "{" + it->first + "}";
			size_t pos = 0;
			while ((pos = ret.find(key, pos)) != std::string::npos)
			{
				ret.replace(pos, key.size(), it->second);
				pos += it->second.size();
			}
		}
		return ret;
	}

	//Number with at most one decimal
	inline std::string number(double val)
	{
		double rounded = std::floor(val * 10 + 0.5) / 10;
		char buf[50];
		if (rounded == std::floor(rounded))
			snprintf(buf, sizeof(buf), "%.0f", rounded);
		else
			snprintf(buf, sizeof(buf), "%.1f", rounded);
		return buf;
	}

	inline params_t param(const std::string& key, const std::string& val)
	{
		params_t ret;
		ret[key] = val;
		return ret;
	}

	inline std::string duration(translate_t translate, double seconds)
	{
		double hours = seconds / 3600;
		if (hours < 48)
		{
			std::string n = number(hours);
			return n == "1" ? msg(translate, "1 hour") : msg(translate, "{n} hours", param("n", n));
		}
		double days = hours / 24;
		if (days < 60)
		{
			std::string n = number(days);
			return n == "1" ? msg(translate, "1 day") : msg(translate, "{n} days", param("n", n));
		}
		if (days < 730)
		{
			return msg(translate, "{n} months", param("n", number(std::floor(days / 30.44 + 0.5))));
		}
		return msg(translate, "{n} years", param("n", number(days / 365.25)));
	}

	//Interval as entered in the settings: number in units of unit_seconds
	inline SInterval parseInterval(std::string text, bool disabled, double unit_seconds)
	{
		if (disabled)
			return SInterval(SInterval::type_disabled);

		while (!text.empty() && isspace(static_cast<unsigned char>(text[0])))
			text.erase(0, 1);
		while (!text.empty() && isspace(static_cast<unsigned char>(text[text.size() - 1])))
			text.erase(text.size() - 1);

		if (text.find('@') != std::string::npos || text.find(';') != std::string::npos)
			return SInterval(SInterval::type_varies);

		if (text.empty() || text.find_first_not_of("0123456789.", text[0] == '-' ? 1 : 0) != std::string::npos
			|| text == "-")
		{
			return SInterval(SInterval::type_none);
		}

		double val = atof(text.c_str());
		if (val == 0)
			return SInterval(SInterval::type_none);
		if (val < 0)
			return SInterval(SInterval::type_disabled);
		return SInterval(SInterval::type_seconds, val*unit_seconds);
	}

	inline int parseCount(std::string text)
	{
		while (!text.empty() && isspace(static_cast<unsigned char>(text[0])))
			text.erase(0, 1);
		while (!text.empty() && isspace(static_cast<unsigned char>(text[text.size() - 1])))
			text.erase(text.size() - 1);

		if (text.empty() || text.size() > 9 || text.find_first_not_of("0123456789") != std::string::npos)
			return -1;
		return atoi(text.c_str());
	}

	struct SSimResult
	{
		SSimResult() : valid(false), fulls_peak(0), incr_peak(0), fine(-1), all(-1), cycle(0),
			per_cycle(0), newest_lost(false) {}

		//False if running it would take too long
		bool valid;
		int fulls_peak;
		int incr_peak;
		//Seconds
		double fine;
		double all;
		double cycle;
		int per_cycle;
		bool newest_lost;
	};

	//Runs the backup schedule and the cleanup of the server (ServerCleanupThread)
	//for one client and volume until the number of backups has settled
	class Simulation
	{
	public:
		//incr, full: seconds, <=0 if disabled
		Simulation(double incr, double full, int max_full, int max_incr, int min_incr, bool image, bool to_last)
			: incr(incr), full(full), max_full(max_full), max_incr(max_incr), min_incr(min_incr),
			image(image), to_last(to_last), work(0) {}

		SSimResult run()
		{
			SSimResult ret;
			if (incr <= 0 && full <= 0)
				return ret;

			double step = incr > 0 ? incr : full;
			//Every cycle_slots-th backup is a full one; 0: only the first one
			int cycle_slots = 1;
			if (incr > 0 && full > 0)
				cycle_slots = (std::max)(1, static_cast<int>(std::ceil(full / incr)));
			else if (incr > 0)
				cycle_slots = 0;

			int per_cycle = cycle_slots > 0 ? cycle_slots - 1 : 1;
			double cycles = max_full + std::ceil(static_cast<double>(max_incr + min_incr) / (std::max)(per_cycle, 1)) + 6;
			double slots = cycles*(std::max)(cycle_slots, 1);
			int measure_slots = 2 * (std::max)(cycle_slots, 1) + 2;
			if (cycle_slots == 0)
			{
				slots = 2 * (max_incr + min_incr) + 50;
				measure_slots = max_incr + min_incr + 10;
			}

			if (slots > max_slots
				|| max_full + max_incr > max_backups)
			{
				return ret;
			}

			ret.valid = true;
			ret.cycle = (std::max)(cycle_slots, 1)*step;
			ret.per_cycle = per_cycle;

			int fine_slots = -1;
			int all_slots = -1;

			for (int t = 0; t < static_cast<int>(slots); ++t)
			{
				std::vector<int> fulls = oldest(true, std::vector<int>());
				int last_full = fulls.empty() ? -1 : backups[fulls[fulls.size() - 1]].t;
				//An incremental image backup needs a full one, an incremental file backup any backup
				bool need_full = image ? last_full == -1 : backups.empty();

				if (need_full || (cycle_slots > 0 && (last_full == -1 || t - last_full >= cycle_slots)))
				{
					backups.push_back(SBackup(t, true, -1));
				}
				else
				{
					backups.push_back(SBackup(t, false, to_last ? backups[backups.size() - 1].t : last_full));
				}

				if (image)
				{
					while (cleanupOneImage()) {}
				}
				else
				{
					cleanupFiles();
				}

				if (work > max_work)
					return SSimResult();

				if (t < static_cast<int>(slots) - measure_slots)
					continue;

				if (backups.empty() || backups[backups.size() - 1].t != t)
					ret.newest_lost = true;

				ret.fulls_peak = (std::max)(ret.fulls_peak, count(true));
				ret.incr_peak = (std::max)(ret.incr_peak, count(false));

				int all = backups.empty() ? 0 : t - backups[0].t;
				int fine = 0;
				for (size_t i = backups.size(); i-- > 0 && backups[i].t == t - fine;)
					++fine;
				fine = (std::max)(fine - 1, 0);

				if (all_slots == -1 || all < all_slots) all_slots = all;
				if (fine_slots == -1 || fine < fine_slots) fine_slots = fine;
			}

			ret.fine = fine_slots*step;
			ret.all = all_slots*step;
			return ret;
		}

	private:
		struct SBackup
		{
			SBackup(int t, bool full, int ref) : t(t), full(full), ref(ref) {}
			//Slot the backup was made in
			int t;
			bool full;
			//Slot of the backup it is based on or -1
			int ref;
		};

		int count(bool p_full)
		{
			work += backups.size();
			int c = 0;
			for (size_t i = 0; i < backups.size(); ++i)
				if (backups[i].full == p_full) ++c;
			return c;
		}

		//Indexes of the backups, oldest first
		std::vector<int> oldest(bool p_full, const std::vector<int>& notit)
		{
			work += backups.size();
			std::vector<int> ret;
			for (size_t i = 0; i < backups.size(); ++i)
			{
				if (backups[i].full == p_full
					&& std::find(notit.begin(), notit.end(), backups[i].t) == notit.end())
				{
					ret.push_back(static_cast<int>(i));
				}
			}
			return ret;
		}

		void remove(int t)
		{
			for (size_t i = 0; i < backups.size(); ++i)
			{
				if (backups[i].t == t)
				{
					backups.erase(backups.begin() + i);
					return;
				}
			}
		}

		//ServerCleanupThread::removeImage with remove_references
		bool removeImage(int t, int del_incr_in_stack)
		{
			work += backups.size();
			if (work > max_work)
				return false;

			std::vector<int> refs;
			for (size_t i = 0; i < backups.size(); ++i)
				if (!backups[i].full && backups[i].ref == t) refs.push_back(backups[i].t);

			for (size_t i = 0; i < refs.size(); ++i)
			{
				if (count(false) - del_incr_in_stack - min_incr <= 0)
					return false;
				if (!removeImage(refs[i], del_incr_in_stack + 1))
					return false;
			}

			remove(t);
			return true;
		}

		//ServerCleanupThread::cleanup_one_imagebackup_client
		bool cleanupOneImage()
		{
			std::vector<int> notit;
			std::vector<int> cand = oldest(true, notit);
			while (static_cast<int>(cand.size()) > max_full)
			{
				int t = backups[cand[0]].t;
				if (removeImage(t, 0)) return true;
				notit.push_back(t);
				cand = oldest(true, notit);
			}

			notit.clear();
			cand = oldest(false, notit);
			while (static_cast<int>(cand.size()) > max_incr)
			{
				int t = backups[cand[0]].t;
				if (removeImage(t, 1)) return true;
				notit.push_back(t);
				cand = oldest(false, notit);
			}
			return false;
		}

		//ServerCleanupThread::cleanup_one_filebackup_client
		void cleanupFiles()
		{
			std::vector<int> cand = oldest(true, std::vector<int>());
			while (static_cast<int>(cand.size()) > max_full
				&& !(cand.size() == 1 && max_incr > 0 && incr > 0 && count(false) == 0))
			{
				remove(backups[cand[0]].t);
				cand = oldest(true, std::vector<int>());
			}

			cand = oldest(false, std::vector<int>());
			while (static_cast<int>(cand.size()) > max_incr)
			{
				remove(backups[cand[0]].t);
				cand = oldest(false, std::vector<int>());
			}
		}

		double incr;
		double full;
		int max_full;
		int max_incr;
		int min_incr;
		bool image;
		bool to_last;
		//Oldest first
		std::vector<SBackup> backups;
		double work;
	};

	//The texts to show for the settings
	inline SSummary describe(const SSettings& settings, translate_t translate)
	{
		SSummary ret;
		const bool image = settings.image;
		const std::string what_full = msg(translate, image ? "full image backups" : "full file backups");
		const std::string what_incr = msg(translate, image ? "incremental image backups" : "incremental file backups");

		double incr = settings.incr.type == SInterval::type_seconds ? settings.incr.seconds : -1;
		double full = settings.full.type == SInterval::type_seconds ? settings.full.seconds : -1;
		bool has_schedule = false;

		if (settings.incr.type == SInterval::type_varies || settings.full.type == SInterval::type_varies)
		{
			ret.info.push_back(msg(translate, "The intervals depend on the time of day or week, so the schedule is not described here."));
		}
		else if (settings.incr.type != SInterval::type_none && settings.full.type != SInterval::type_none)
		{
			has_schedule = incr > 0 || full > 0;
			params_t p;
			if (incr > 0) p["incr"] = duration(translate, incr);
			if (incr > 0 && full > 0 && full <= incr)
			{
				ret.info.push_back(msg(translate, image
					? "Every scheduled image backup is a full image backup, about every {incr}."
					: "Every scheduled file backup is a full file backup, about every {incr}.", p));
				p["full"] = duration(translate, full);
				ret.warn.push_back(msg(translate, image
					? "The interval for full image backups ({full}) is not longer than the interval for incremental image backups ({incr}). No incremental image backups are made and a full image backup runs every {incr}."
					: "The interval for full file backups ({full}) is not longer than the interval for incremental file backups ({incr}). No incremental file backups are made and a full file backup runs every {incr}.", p));
			}
			else if (incr > 0 && full > 0)
			{
				p["full"] = duration(translate, std::ceil(full / incr)*incr);
				ret.info.push_back(msg(translate, image
					? "A full image backup runs about every {full} and an incremental image backup about every {incr} in between."
					: "A full file backup runs about every {full} and an incremental file backup about every {incr} in between.", p));
			}
			else if (incr > 0)
			{
				ret.info.push_back(msg(translate, image
					? "An incremental image backup runs about every {incr}. Full image backups are not scheduled, only the first image backup of a volume is a full one."
					: "An incremental file backup runs about every {incr}. Full file backups are not scheduled, only the first file backup is a full one.", p));
			}
			else if (full > 0)
			{
				p["full"] = duration(translate, full);
				ret.info.push_back(msg(translate, image
					? "A full image backup runs about every {full}. Incremental image backups are disabled."
					: "A full file backup runs about every {full}. Incremental file backups are disabled.", p));
			}
			else
			{
				ret.info.push_back(msg(translate, image ? "No image backups are scheduled." : "No file backups are scheduled."));
			}

			if (has_schedule)
				ret.info.push_back(msg(translate, "Backups only start while the client is online and inside the backup window, so they can run later than that."));
		}

		if (settings.max_full < 0 || settings.min_full < 0 || settings.max_incr < 0 || settings.min_incr < 0)
			return ret;

		params_t counts;
		counts["max_full"] = number(settings.max_full);
		counts["max_incr"] = number(settings.max_incr);
		counts["min_full"] = number(settings.min_full);
		counts["min_incr"] = number(settings.min_incr);

		ret.info.push_back(msg(translate, image
			? "The server keeps at most {max_full} full and {max_incr} incremental image backups per volume. An incremental image backup can only be restored together with the image backups it is based on, so those are not deleted before it."
			: "The server keeps at most {max_full} full and {max_incr} incremental file backups. Each of them can be restored on its own.", counts));

		bool full_used = has_schedule && (full > 0 || incr > 0);
		bool incr_used = has_schedule && incr > 0 && !(full > 0 && full <= incr);

		const char* warn_min_max = "The minimal number of {what} ({min}) is larger than the maximal number ({max}). Never more than {max} are kept, and none of them can be deleted if the backup storage runs out of space.";
		const char* warn_max_zero = "The maximal number of {what} is 0, so they are deleted again by the next cleanup.";
		if (settings.min_full > settings.max_full)
		{
			params_t p = param("what", what_full);
			p["min"] = counts["min_full"];
			p["max"] = counts["max_full"];
			ret.warn.push_back(msg(translate, warn_min_max, p));
		}
		if (settings.min_incr > settings.max_incr)
		{
			params_t p = param("what", what_incr);
			p["min"] = counts["min_incr"];
			p["max"] = counts["max_incr"];
			ret.warn.push_back(msg(translate, warn_min_max, p));
		}
		if (settings.max_full == 0 && full_used)
			ret.warn.push_back(msg(translate, warn_max_zero, param("what", what_full)));
		if (settings.max_incr == 0 && incr_used)
			ret.warn.push_back(msg(translate, warn_max_zero, param("what", what_incr)));

		SSimResult sim;
		if (has_schedule && image && full <= 0)
		{
			ret.info.push_back(msg(translate, "All incremental image backups are based, directly or through earlier ones, on the first full image backup, so that one is never deleted."));
		}
		else if (has_schedule)
		{
			std::vector<bool> styles = settings.to_last;
			if (!image || styles.empty())
				styles.assign(1, false);

			for (size_t i = 0; i < styles.size(); ++i)
			{
				SSimResult curr = Simulation(incr, full, settings.max_full, settings.max_incr,
					settings.min_incr, image, styles[i]).run();

				if (!curr.valid)
				{
					sim = SSimResult();
					break;
				}

				if (!sim.valid)
				{
					sim = curr;
				}
				else
				{
					sim.fulls_peak = (std::max)(sim.fulls_peak, curr.fulls_peak);
					sim.incr_peak = (std::max)(sim.incr_peak, curr.incr_peak);
					sim.fine = (std::min)(sim.fine, curr.fine);
					sim.all = (std::min)(sim.all, curr.all);
					sim.newest_lost = sim.newest_lost || curr.newest_lost;
				}
			}
		}

		if (sim.valid)
		{
			double step = incr > 0 ? incr : full;
			//Without scheduled full backups the first one stays, however old it gets
			if (full <= 0) sim.all = sim.fine;

			params_t p;
			p["fine"] = duration(translate, sim.fine);
			p["all"] = duration(translate, sim.all);
			if (sim.all > 0 && sim.fine > 0 && sim.fine + step < sim.all)
				ret.info.push_back(msg(translate, "If every scheduled backup runs, every backup of the last {fine} can be restored. Further back only some of them are left, the oldest one is about {all} old.", p));
			else if (sim.all > 0)
				ret.info.push_back(msg(translate, "If every scheduled backup runs, the backups reach back about {all}.", p));

			if (image && sim.newest_lost && incr_used && settings.max_incr > 0)
			{
				ret.warn.push_back(msg(translate, "With these settings the cleanup deletes the newest incremental image backups instead of the oldest ones. Incremental image backups are based on each other here, and one that others are based on is only deleted together with them. Keep at least {n} incremental image backups or base them on the last full image backup (incremental image style).",
					param("n", number(sim.per_cycle))));
			}

			if (image && sim.fulls_peak > settings.max_full && settings.max_full > 0)
			{
				params_t pw = param("max", counts["max_full"]);
				pw["min_incr"] = counts["min_incr"];
				pw["n"] = number(sim.fulls_peak);
				ret.warn.push_back(msg(translate, "The maximal number of full image backups ({max}) cannot always be kept. A full image backup is only deleted together with the incremental image backups based on it, and the minimal number of incremental image backups ({min_incr}) prevents that. Expect up to {n} full image backups.", pw));
			}
			if (image && sim.incr_peak > settings.max_incr && settings.max_incr > 0 && !sim.newest_lost)
			{
				params_t pw = param("max", counts["max_incr"]);
				pw["n"] = number(sim.incr_peak);
				ret.warn.push_back(msg(translate, "The maximal number of incremental image backups ({max}) cannot always be kept, because an incremental image backup that others are based on is only deleted together with them. Expect up to {n} incremental image backups.", pw));
			}
		}

		if (settings.min_full <= settings.max_full && settings.min_incr <= settings.max_incr
			&& (settings.min_full < settings.max_full || settings.min_incr < settings.max_incr))
		{
			ret.info.push_back(msg(translate, "If the backup storage runs out of space, the oldest backups are deleted earlier, down to {min_full} full and {min_incr} incremental ones.", counts));
		}

		return ret;
	}
}
