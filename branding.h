/*************************************************************************
*    UrBackup - Client/Server backup system
*    Copyright (C) 2026 AI Vector
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

/*
 * Product name shown in message box captions and the tray icon tooltip.
 * Defined in one place so a distribution that ships the client under its
 * own name changes it here (or with a compiler define) instead of in every
 * dialog. Translated sentences that mention UrBackup are not affected.
 */
#ifndef URBACKUP_APP_NAME
#define URBACKUP_APP_NAME wxT("UrBackup")
#endif

#ifndef URBACKUP_APP_NAME_CLIENT
#define URBACKUP_APP_NAME_CLIENT wxT("UrBackup Client")
#endif
