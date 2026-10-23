#include "stdafx.h"
#include <winreg.h>
#include <iomanip>
#include <filesystem>
#include "resource.h"
#include "prefs.h"
#include "../inipp/inipp.h"

#pragma warning(disable: 6031)

Prefs prefs;

void Prefs::open()
{
	char* p = getenv("blitzpath");
	if (!p)
	{
		AfxMessageBox("blitzpath environment variable not found!", MB_TOPMOST | MB_SETFOREGROUND | MB_ICONERROR);
		ExitProcess(0);
	}
	homeDir = p;

	setDefault();

	std::ifstream in((homeDir + "/cfg/blitzide.ini").c_str(), std::ios::in);
	if (!in.good()) return;

	in.seekg(0, std::ios::end);
	if (in.tellg() == 0)
	{
		AfxMessageBox("blitzide.ini is empty!\nDefaults will be set.", MB_TOPMOST | MB_SETFOREGROUND | MB_ICONERROR);
		return;
	}
	in.seekg(0, std::ios::beg);

	inipp::Ini<char> ini;
	ini.parse(in);

	inipp::get_value(ini.sections["COMPILER"], "Preprocess", prg_preprocess);
	inipp::get_value(ini.sections["COMPILER"], "Debug", prg_debug);
	inipp::get_value(ini.sections["COMPILER"], "NoLAA", prg_nolaa);
	inipp::get_value(ini.sections["COMPILER"], "LastBuild", prg_lastbuild);
	inipp::get_value(ini.sections["COMPILER"], "CommandLine", cmd_line);

	std::string unprocessedRect;

	inipp::get_value(ini.sections["WINDOW"], "Maximized", win_maximized);
	inipp::get_value(ini.sections["WINDOW"], "NoToolbar", win_notoolbar);
	inipp::get_value(ini.sections["WINDOW"], "WindowRect", unprocessedRect);

	sscanf(unprocessedRect.c_str(), "%ld %ld %ld %ld", &win_rect.left, &win_rect.top, &win_rect.right, &win_rect.bottom);

	inipp::get_value(ini.sections["FONTS"], "EditorFont", font_editor);
	inipp::get_value(ini.sections["FONTS"], "EditorFontSize", font_editor_height);
	inipp::get_value(ini.sections["FONTS"], "TabsFont", font_tabs);
	inipp::get_value(ini.sections["FONTS"], "TabsFontSize", font_tabs_height);
	inipp::get_value(ini.sections["FONTS"], "DebugFont", font_debug);
	inipp::get_value(ini.sections["FONTS"], "DebugFontSize", font_debug_height);

	std::string unprocessedBackground, unprocessedString, unprocessedIdent, unprocessedKeyword, unprocessedComment, unprocessedDigit, unprocessedDefault;

	inipp::get_value(ini.sections["COLORS"], "Background", unprocessedBackground);
	inipp::get_value(ini.sections["COLORS"], "String", unprocessedString);
	inipp::get_value(ini.sections["COLORS"], "Ident", unprocessedIdent);
	inipp::get_value(ini.sections["COLORS"], "Keyword", unprocessedKeyword);
	inipp::get_value(ini.sections["COLORS"], "Comment", unprocessedComment);
	inipp::get_value(ini.sections["COLORS"], "Digit", unprocessedDigit);
	inipp::get_value(ini.sections["COLORS"], "Default", unprocessedDefault);

	int r = 0, g = 0, b = 0;

	sscanf(unprocessedBackground.c_str(), "%d %d %d", &r, &g, &b);
	rgb_bkgrnd = RGB(r, g, b);
	sscanf(unprocessedString.c_str(), "%d %d %d", &r, &g, &b);
	rgb_string = RGB(r, g, b);
	sscanf(unprocessedIdent.c_str(), "%d %d %d", &r, &g, &b);
	rgb_ident = RGB(r, g, b);
	sscanf(unprocessedKeyword.c_str(), "%d %d %d", &r, &g, &b);
	rgb_keyword = RGB(r, g, b);
	sscanf(unprocessedComment.c_str(), "%d %d %d", &r, &g, &b);
	rgb_comment = RGB(r, g, b);
	sscanf(unprocessedDigit.c_str(), "%d %d %d", &r, &g, &b);
	rgb_digit = RGB(r, g, b);
	sscanf(unprocessedDefault.c_str(), "%d %d %d", &r, &g, &b);
	rgb_default = RGB(r, g, b);

	inipp::get_value(ini.sections["EDITOR"], "TabSpaces", edit_tabs);
	inipp::get_value(ini.sections["EDITOR"], "BackupCount", edit_backup);
	inipp::get_value(ini.sections["EDITOR"], "ToolbarImage", img_toolbar);
	inipp::get_value(ini.sections["EDITOR"], "NoBackup", noBackup);

	std::string recentFile;
	for (int i = 1; i < 11; i++)
	{
		inipp::get_value(ini.sections["RECENT_FILES"], "File" + itoa(i), recentFile);
		if (recentFile.size() == 0) continue;
		recentFiles.push_back(recentFile);
	}
	createFonts();
	in.close();
}

void Prefs::close()
{
	//the horror
	std::fstream out((homeDir + "/cfg/blitzide.ini").c_str(), std::ios::out | std::ios::trunc);
	if (!out.good()) return;

	inipp::Ini<char> ini;

	auto& compilerSection = ini.sections["COMPILER"];
	compilerSection.insert(std::make_pair("Preprocess", boolToString(prg_preprocess)));
	compilerSection.insert(std::make_pair("Debug", boolToString(prg_debug)));
	compilerSection.insert(std::make_pair("NoLAA", boolToString(prg_nolaa)));
	compilerSection.insert(std::make_pair("LastBuild", prg_lastbuild));
	compilerSection.insert(std::make_pair("CommandLine", cmd_line));

	auto& windowSection = ini.sections["WINDOW"];
	windowSection.insert(std::make_pair("Maximized", boolToString(win_maximized)));
	windowSection.insert(std::make_pair("NoToolbar", boolToString(win_notoolbar)));
	windowSection.insert(std::make_pair("WindowRect", std::to_string(win_rect.left) + " " + std::to_string(win_rect.top) + " " + std::to_string(win_rect.right) + " " + std::to_string(win_rect.bottom)));

	auto& fontsSection = ini.sections["FONTS"];
	fontsSection.insert(std::make_pair("EditorFont", font_editor));
	fontsSection.insert(std::make_pair("EditorFontSize", std::to_string(font_editor_height)));
	fontsSection.insert(std::make_pair("TabsFont", font_tabs));
	fontsSection.insert(std::make_pair("TabsFontSize", std::to_string(font_tabs_height)));
	fontsSection.insert(std::make_pair("DebugFont", font_debug));
	fontsSection.insert(std::make_pair("DebugFontSize", std::to_string(font_debug_height)));

	auto& colorsSection = ini.sections["COLORS"];
	colorsSection.insert(std::make_pair("Background", std::to_string(GetRValue(rgb_bkgrnd)) + " " + std::to_string(GetGValue(rgb_bkgrnd)) + " " + std::to_string(GetBValue(rgb_bkgrnd))));
	colorsSection.insert(std::make_pair("String", std::to_string(GetRValue(rgb_string)) + " " + std::to_string(GetGValue(rgb_string)) + " " + std::to_string(GetBValue(rgb_string))));
	colorsSection.insert(std::make_pair("Ident", std::to_string(GetRValue(rgb_ident)) + " " + std::to_string(GetGValue(rgb_ident)) + " " + std::to_string(GetBValue(rgb_ident))));
	colorsSection.insert(std::make_pair("Keyword", std::to_string(GetRValue(rgb_keyword)) + " " + std::to_string(GetGValue(rgb_keyword)) + " " + std::to_string(GetBValue(rgb_keyword))));
	colorsSection.insert(std::make_pair("Comment", std::to_string(GetRValue(rgb_comment)) + " " + std::to_string(GetGValue(rgb_comment)) + " " + std::to_string(GetBValue(rgb_comment))));
	colorsSection.insert(std::make_pair("Digit", std::to_string(GetRValue(rgb_digit)) + " " + std::to_string(GetGValue(rgb_digit)) + " " + std::to_string(GetBValue(rgb_digit))));
	colorsSection.insert(std::make_pair("Default", std::to_string(GetRValue(rgb_default)) + " " + std::to_string(GetGValue(rgb_default)) + " " + std::to_string(GetBValue(rgb_default))));

	auto& editorSection = ini.sections["EDITOR"];
	editorSection.insert(std::make_pair("TabSpaces", std::to_string(edit_tabs)));
	editorSection.insert(std::make_pair("BackupCount", std::to_string(edit_backup)));
	editorSection.insert(std::make_pair("ToolbarImage", img_toolbar));
	editorSection.insert(std::make_pair("NoBackup", boolToString(noBackup)));

	auto& recentFilesSection = ini.sections["RECENT_FILES"];
	for (int i = 1; i < 11; i++)
	{
		recentFilesSection.insert(std::make_pair("File" + itoa(i), i <= recentFiles.size() ? recentFiles[i - 1] : ""));
	}

	ini.generate(out);
}

void Prefs::setDefault()
{
	prg_preprocess = false;
	prg_debug = true;
	prg_nolaa = false;

	win_rect.left = win_rect.top = 0;
	win_rect.right = 800; win_rect.bottom = 600;
	win_maximized = false;
	win_notoolbar = false;

	font_editor = "consolas";
	font_editor_height = 14;
	font_debug = "consolas";
	font_debug_height = 14;
	font_tabs = "consolas";
	font_tabs_height = 10;

	rgb_bkgrnd = RGB(15, 15, 15);
	rgb_string = RGB(0, 255, 102);
	rgb_ident = RGB(255, 255, 255);
	rgb_keyword = RGB(20, 105, 200);
	rgb_comment = RGB(0, 180, 0);
	rgb_digit = RGB(255, 255, 255);
	rgb_default = RGB(238, 238, 238);

	edit_tabs = 4;
	edit_backup = 2;

	img_toolbar = "toolbar.bmp";

	recentFiles.clear();

	createFonts();
}

void Prefs::createFonts()
{
	editFont.Detach();
	tabsFont.Detach();
	debugFont.Detach();
	conFont.Detach();

	editFont.CreatePointFont(font_editor_height * 10, font_editor.c_str());
	tabsFont.CreatePointFont(font_tabs_height * 10, font_tabs.c_str());
	debugFont.CreatePointFont(font_debug_height * 10, font_debug.c_str());
	conFont.CreatePointFont(80, "courier");
}

std::string Prefs::boolToString(bool value)
{
	return value ? "true" : "false";
}