/*
 * lst - A shitty file manager
 * Copyright (C) 2021 ItzAfroBoy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE
#define _XOPEN_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#include "lst.h"
#include "ini.h"

void onExit()
{
	disableRawMode();
	freeRows();
	if (fmc.exts.print == 1)
		printf("%s\r\n", fmc.dirname);
	free(fmc.dirname);
}

void showHelp()
{
	printf("lst [options] <path>\r\n");
	printf("a shitty file manager\r\n\r\n");
	printf("OPTIONS\r\n");
	printf("\t-h\t:: show help\r\n");
	printf("\t-v\t:: show version\r\n");
	printf("\t-t\t:: show tildes\r\n");
	printf("\t-n\t:: show numbers\r\n");
	printf("\t-i\t:: show file info\r\n");
	printf("\t-p\t:: show final path at exit\r\n\r\n");
	printf("OPERATIONS\r\n");
	printf("\t q\t:: exit lst\r\n");
	printf("\t r\t:: rename a file\r\n");
	printf("\t d\t:: delete a file\r\n");
	printf("\t c\t:: create a new file\r\n");
	printf("\t^c\t:: create a new folder\r\n");
	exit(0);
}

void showVersion()
{
	printf("lst version: %s\r\n", LST_VERSION);
	printf("ini version: %s\r\n", INI_VERSION);
	exit(0);
}

void die(char *s)
{
	write(STDOUT_FILENO, "\e[2J\e[H", 7);
	perror((const char *)s);
	free(s);
	exit(1);
}

void enableRawMode()
{
	if (tcgetattr(STDIN_FILENO, &fmc.std) == -1)
		die(itoa(__LINE__, NULL));

	atexit(onExit);

	struct termios raw = fmc.std;
	raw.c_cflag |= ~(CS8);
	raw.c_oflag &= ~(OPOST);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die(itoa(__LINE__, NULL));
}

void disableRawMode()
{
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &fmc.std) == -1)
		die(itoa(__LINE__, NULL));

	write(STDOUT_FILENO, "\r\e[?25h", 7);
}

void handleSigWinCh(int signo)
{
	getScreenSize(&fmc.rows, &fmc.cols);
	fmc.rows -= 2;
	refreshScreen();
}

int getScreenSize(int *rows, int *cols)
{
	struct winsize ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
	{
		if (write(STDOUT_FILENO, "\e[999C\e[999B", 12) != 12)
			return -1;

		return getCursorPos(rows, cols);
	}
	else
	{
		*cols = ws.ws_col;
		*rows = ws.ws_row;

		return 0;
	}
}

int getCursorPos(int *rows, int *cols)
{
	char buf[32];
	unsigned int i = 0;

	if (write(STDOUT_FILENO, "\e[6n", 4) != 4)
		return -1;

	while (i < sizeof(buf) - 1)
	{
		if (read(STDIN_FILENO, &buf[i], 1) != 1)
			break;
		if (buf[i] == 'R')
			break;
		i++;
	}

	buf[i] = '\0';
	if (buf[0] != '\e' || buf[1] != '[')
		return -1;
	if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
		return -1;

	return 0;
}

int getFileSize(char *filename)
{
	struct stat statbuf;
	char file[80];

	memset(file, 0, 80);
	appendPath(filename, file, sizeof(file));

	if (stat(file, &statbuf) == -1)
		return 0;
	else if (S_ISDIR(statbuf.st_mode))
		return 0;

	FILE *fp = fopen(file, "r");
	if (!fp)
		return 0;

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fclose(fp);

	if (!(size > 0))
		return 0;

	return size;
}

char *getCurrentTime()
{
	size_t bufsize = 6;
	char *buf = malloc(bufsize);
	buf[0] = '\0';

	time_t rawtime;
	struct tm *ti;

	time(&rawtime);
	ti = localtime(&rawtime);
	strftime(buf, 80, "%R", ti);
	return buf;
}

int getLineNoLen(char *buf, int size, int lineNo)
{
	int len;
	if (fmc.numrows >= 0 && fmc.numrows < 10)
		len = snprintf(buf, size, "%d ", lineNo);
	else if (fmc.numrows >= 10 && fmc.numrows < 100)
		len = snprintf(buf, size, "%2d ", lineNo);
	else if (fmc.numrows >= 100 && fmc.numrows < 1000)
		len = snprintf(buf, size, "%3d ", lineNo);
	else if (fmc.numrows >= 1000 && fmc.numrows < 10000)
		len = snprintf(buf, size, "%4d ", lineNo);
	else if (fmc.numrows >= 10000 && fmc.numrows < 100000)
		len = snprintf(buf, size, "%5d ", lineNo);

	return len;
}

int readKey()
{
	int _r;
	char c;

	while ((_r = read(STDIN_FILENO, &c, 1)) != 1)
	{
		if (_r == -1 && errno != EAGAIN)
			die(itoa(__LINE__, NULL));
		else if (_r == 0)
			return 0;
	}

	if (c == '\e')
	{
		char seq[3];
		if (read(STDIN_FILENO, &seq[0], 1) != 1)
			return '\e';
		if (read(STDIN_FILENO, &seq[1], 1) != 1)
			return '\e';

		if (seq[0] == '[')
		{
			if (seq[1] >= '0' && seq[1] <= '9')
			{
				if (read(STDIN_FILENO, &seq[2], 1) != 1)
					return '\e';
				if (seq[2] == '~')
				{
					switch (seq[1])
					{
					case '3':
						return DEL_KEY;
					case '5':
						return PAGE_UP;
					case '6':
						return PAGE_DOWN;
					}
				}
			}
			else
			{
				switch (seq[1])
				{
				case 'A':
					return ARROW_UP;
				case 'B':
					return ARROW_DOWN;
				case 'C':
					return ARROW_RIGHT;
				case 'D':
					return ARROW_LEFT;
				}
			}
		}

		return '\e';
	}
	else
	{
		return c;
	}
}

void processKey()
{
	int c = readKey();
	switch (c)
	{
	case '\r':
	case ARROW_RIGHT:
		changeDir(FORWARD);
		break;

	case BACKSPACE:
	case ARROW_LEFT:
		changeDir(BACK);
		break;

	case CTRL_KEY('c'):
	{
		char *ans = prompt("New Dir: %s");
		if (ans)
			createEntry(ans, T_DIR);
	}
	break;

	case ARROW_UP:
	case ARROW_DOWN:
		moveCursor(c);
		break;

	case 'q':
	case '\e':
		write(STDOUT_FILENO, "\e[2J\e[H", 7);
		exit(0);
		break;

	case 'd':
		deleteEntry();
		break;

	case 'r':
	{
		char *ans = prompt("New Name: %s");
		if (ans)
			renameEntry(ans);
	}
	break;

	case 'c':
	{
		char *ans = prompt("New File: %s");
		if (ans)
			createEntry(ans, T_FILE);
	}
	break;

	default:
	{
		int cur = fmc.cy;
		changeDir(REFRESH);
		if (cur < fmc.numrows)
			fmc.cy = cur;
		else
			fmc.cy = fmc.numrows - 1;
	}
	}
}

void moveCursor(int key)
{
	switch (key)
	{
	case ARROW_UP:
		if (fmc.cy != 0)
			fmc.cy--;
		else if (fmc.cy == 0)
			fmc.cy = fmc.numrows - 1;
		break;

	case ARROW_DOWN:
		if (fmc.cy < fmc.numrows - 1)
			fmc.cy++;
		else if (fmc.cy == fmc.numrows - 1)
			fmc.cy = 0;
		break;
	}
}

void scroll()
{
	if (fmc.cy < fmc.rowoff)
		fmc.rowoff = fmc.cy;
	if (fmc.cy >= fmc.rowoff + fmc.rows)
		fmc.rowoff = fmc.cy - fmc.rows + 1;
}

void wbWrite(struct wbuf *wb, const char *s, int len)
{
	char *new = realloc(wb->b, wb->len + len);
	if (new == NULL)
		return;

	memcpy(&new[wb->len], s, len);

	wb->b = new;
	wb->len += len;
}

void wbFree(struct wbuf *wb)
{
	free(wb->b);
}

void freeRows()
{
	for (int i = 0; i < fmc.numrows; i++)
		free(fmc.row[i].chars);

	free(fmc.row);
}

void insertRow(char *s, size_t len)
{
	fmc.row = realloc(fmc.row, sizeof(fmrow) * (fmc.numrows + 1));
	int at = fmc.numrows;

	fmc.row[at].size = len;
	fmc.row[at].chars = malloc(len + 1);
	memcpy(fmc.row[at].chars, s, len);
	fmc.row[at].chars[len] = '\0';
	fmc.numrows++;
}

void drawRows(struct wbuf *wb)
{
	int y;
	for (y = 0; y < fmc.rows; y++)
	{
		int filerow = y + fmc.rowoff;
		char lineNo[7];
		int lineNoLen = getLineNoLen(lineNo, sizeof(lineNo), filerow + 1);

		if (filerow >= fmc.numrows)
		{
			if (fmc.exts.tildes == 1)
				wbWrite(wb, "\e[2m~\e[m", 8);
		}
		else
		{
			int len = fmc.row[filerow].size;

			if (fmc.exts.numbers == 1)
				wbWrite(wb, lineNo, lineNoLen);

			if (len > fmc.cols)
				len = fmc.cols;

			if (fmc.cy == filerow)
				highlightLine(wb, fmc.row[filerow].chars, len);
			else
				wbWrite(wb, fmc.row[filerow].chars, len);

			if (fmc.exts.info == 1)
			{
				char buf[80];
				char path[80];
				struct stat statbuf;
				int offset = fmc.exts.numbers == 1 ? lineNoLen : 0;
				int buflen;

				appendPath(fmc.row[filerow].chars, path, sizeof(path));

				if (stat(path, &statbuf) == -1)
					buflen = snprintf(buf, sizeof(buf), ">");
				else if (S_ISREG(statbuf.st_mode))
					buflen = snprintf(buf, sizeof(buf), "%.1f KB", (float)getFileSize(fmc.row[filerow].chars) / 1024);
				else
					buflen = snprintf(buf, sizeof(buf), ">");

				while (len < fmc.cols)
				{
					if (fmc.cols - len == buflen + offset)
					{
						if (fmc.cy == filerow)
							highlightLine(wb, buf, buflen);
						else
							wbWrite(wb, buf, buflen);
						break;
					}
					else
					{
						wbWrite(wb, " ", 1);
						len++;
					}
				}
			}
		}

		wbWrite(wb, "\r\n\e[K", 5);
	}
}

void drawStatusBar(struct wbuf *wb)
{
	char status[100], rstatus[100];
	char *dir = truncateDir(fmc.dirname);
	int len, rlen;

	len = snprintf(status, sizeof(status), "%s - %d dirs", dir, fmc.numrows);
	if (fmc.numrows != 0)
		rlen = snprintf(rstatus, sizeof(rstatus), "%d %s | %d/%d", getFileSize(fmc.row[fmc.cy].chars), "Bytes", fmc.cy + 1, fmc.numrows);
	else
		rlen = snprintf(rstatus, sizeof(rstatus), "%d %s | %d/%d", 0, "Bytes", fmc.cy + 1, fmc.numrows);

	if (len > fmc.cols)
		len = fmc.cols;

	wbWrite(wb, "\e[30;41m", 8);
	wbWrite(wb, status, len);

	while (len < fmc.cols)
	{
		if (fmc.cols - len == rlen)
		{
			wbWrite(wb, rstatus, rlen);
			break;
		}
		else
		{
			wbWrite(wb, " ", 1);
			len++;
		}
	}

	free(dir);
	wbWrite(wb, "\e[m\r\n", 5);
}

void drawMessageBar(struct wbuf *wb)
{
	wbWrite(wb, "\e[K", 3);

	char *curtime = getCurrentTime();
	int len = strlen(fmc.message);
	int rlen = strlen(curtime);

	if (len > fmc.cols)
		len = fmc.cols;

	if (len && time(NULL) - fmc.msgtime < 5)
		wbWrite(wb, fmc.message, len);
	else
	{
		len = 0;

		memset(fmc.message, 0, 80);
	}

	while (len < fmc.cols)
	{
		if (fmc.cols - len == rlen)
		{
			wbWrite(wb, curtime, rlen);
			break;
		}
		else
		{
			wbWrite(wb, " ", 1);
			len++;
		}
	}

	free(curtime);
}

void setMessage(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(fmc.message, sizeof(fmc.message), fmt, ap);
	va_end(ap);
	fmc.msgtime = time(NULL);
}

char *prompt(char *prompt)
{
	size_t bufsize = 128;
	char *buf = malloc(bufsize);
	size_t buflen = 0;
	buf[0] = '\0';

	while (1)
	{
		setMessage(prompt, buf);
		refreshScreen();

		int c = readKey();
		if (c == BACKSPACE)
		{
			if (buflen != 0)
				buf[--buflen] = '\0';
			else
				buf[buflen] = '\0';
		}
		else if (c == '\e')
		{
			setMessage("");
			free(buf);

			return NULL;
		}
		else if (c == '\r')
		{
			if (buflen != 0)
			{
				setMessage("");
				return buf;
			}
		}
		else if (!iscntrl(c) && c < 128)
		{
			if (buflen == bufsize - 1)
			{
				bufsize *= 2;
				buf = realloc(buf, bufsize);
			}

			buf[buflen++] = c;
			buf[buflen] = '\0';
		}
	}
}

char *truncateDir(char *_dir)
{
	char **tokens = malloc(sizeof(char *) * TOK_BUFSIZE);
	char *dir = malloc(sizeof(char *) * TOK_BUFSIZE * 2);
	char *token;
	char buf[80];
	int position = 0;
	dir[0] = '\0';

	memset(buf, 0, 80);
	snprintf(buf, sizeof(buf), "%s", _dir);

	token = strtok(buf, "/");
	while (token != NULL)
	{
		tokens[position] = token;
		position++;
		token = strtok(NULL, "/");
	}

	tokens[position] = NULL;

	if (position == 0)
	{
		strcat(dir, "/");
	}
	else if (position == 1)
	{
		strcat(dir, "/");
		strcat(dir, tokens[0]);
	}
	else
	{
		strcat(dir, tokens[position - 2]);
		strcat(dir, "/");
		strcat(dir, tokens[position - 1]);
	}

	free(tokens);
	return dir;
}

void appendPath(char *path, char *buf, int bufsize)
{
	char *sep;
	if (strcmp(fmc.dirname, "/") == 0)
		sep = "";
	else
		sep = "/";

	snprintf(buf, bufsize, "%s%s%s", fmc.dirname, sep, path);
}

void highlightLine(struct wbuf *wb, const char *s, int len)
{
	wbWrite(wb, "\e[30;44m", 8);
	wbWrite(wb, s, len);
	wbWrite(wb, "\e[m", 3);
}

void changeDir(int op)
{
	char dirname[80];
	memset(dirname, 0, 80);

	if (op == FORWARD)
		appendPath(fmc.row[fmc.cy].chars, dirname, sizeof(dirname));
	else if (op == REFRESH)
	{
		snprintf(dirname, sizeof(dirname), "%s", fmc.dirname);
	}
	else if (op == BACK)
	{
		char **tokens = malloc(sizeof(char *) * TOK_BUFSIZE);
		char *token;
		char buf[80];
		char dir[80];
		int position = 0;

		memset(buf, 0, 80);
		memset(dir, 0, 80);
		snprintf(dir, sizeof(dir), "%s", fmc.dirname);

		token = strtok(dir, "/");
		while (token != NULL)
		{
			tokens[position] = token;
			position++;
			token = strtok(NULL, "/");
		}

		tokens[position] = NULL;
		for (int i = 0; i < position - 1; i++)
		{
			strcat(buf, "/");
			strcat(buf, tokens[i]);
		}

		if (buf[0] == 0)
			strcat(buf, "/");

		snprintf(dirname, sizeof(dirname), "%s", buf);
		free(tokens);
	}

	freeRows();

	fmc.cy = 0;
	fmc.rowoff = 0;
	fmc.numrows = 0;
	fmc.row = NULL;

	openDir(dirname);
}

void openDir(char *dirname)
{
	free(fmc.dirname);

	struct stat statbuf;
	fmc.dirname = strdup(dirname);

	if (stat(dirname, &statbuf) == -1)
		die(itoa(__LINE__, NULL));
	else if (S_ISREG(statbuf.st_mode))
		openFile();

	DIR *dir = opendir(dirname);
	struct dirent *entry;

	while ((entry = readdir(dir)))
	{
		int _c = strcmp(entry->d_name, ".");
		int _cc = strcmp(entry->d_name, "..");

		if (_c != 0 && _cc != 0)
			insertRow(entry->d_name, strlen(entry->d_name));
	}

	closedir(dir);
}

void openFile()
{
	char *args[3];
	args[0] = "txt";
	args[1] = fmc.dirname;
	args[2] = '\0';

	disableRawMode();
	execvp(args[0], args);
	die(itoa(__LINE__, NULL));
}

void renameEntry(char *newname)
{
	char buf[80];
	char buf2[80];

	memset(buf, 0, 80);
	memset(buf2, 0, 80);
	appendPath(fmc.row[fmc.cy].chars, buf, sizeof(buf));
	appendPath(newname, buf2, sizeof(buf2));
	rename(buf, buf2);
	changeDir(REFRESH);
	for (int i = 0; i < fmc.numrows; i++)
	{
		if (strcmp(fmc.row[i].chars, newname) == 0)
		{
			fmc.cy = i;
			break;
		}
	}
	free(newname);
}

void deleteEntry()
{
	int cur = fmc.cy;
	char buf[80];

	memset(buf, 0, 80);
	appendPath(fmc.row[fmc.cy].chars, buf, sizeof(buf));
	remove(buf);
	changeDir(REFRESH);
	if (fmc.numrows != 0)
		fmc.cy = cur > fmc.numrows - 1 ? cur - 1 : cur;
}

void createEntry(char *entry, int type)
{
	struct stat statbuf;
	char buf[80];

	memset(buf, 0, 80);
	appendPath(entry, buf, sizeof(buf));

	if (type == T_FILE)
	{
		FILE *fp = NULL;
		if (stat(buf, &statbuf) == -1)
			fp = fopen(buf, "w");
		else if (S_ISREG(statbuf.st_mode))
		{
			char *ans = prompt("Are you sure you want to overwrite?: %s [y/n]");
			if (strcmp(ans, "y") == 0)
				fp = fopen(buf, "w");
			else if (strcmp(ans, "n") == 0)
				setMessage("Overwrite aborted");
			free(ans);
		}

		if (fp)
			fclose(fp);
	}
	else if (type == T_DIR)
	{
		if (stat(buf, &statbuf) == -1)
			mkdir(buf, 0777);
		else if (S_ISDIR(statbuf.st_mode))
			setMessage("Directory already exists");
	}

	changeDir(REFRESH);
	for (int i = 0; i < fmc.numrows; i++)
	{
		if (strcmp(fmc.row[i].chars, entry) == 0)
		{
			fmc.cy = i;
			break;
		}
	}
	free(entry);
}

char *itoa(int num, char *buf)
{
	if (!buf)
	{
		char *_buf = malloc(sizeof(char *) * 64);
		snprintf(_buf, sizeof(buf), "%d", num);
		return _buf;
	}
	else
	{
		sprintf(buf, "%d", num);
		return buf;
	}
}

void refreshScreen()
{
	char buf[32];
	struct wbuf wb = WBUF_INIT;

	scroll();
	wbWrite(&wb, "\e[?25l\e[H\e[K", 12);

	drawRows(&wb);
	drawStatusBar(&wb);
	drawMessageBar(&wb);

	snprintf(buf, sizeof(buf), "\e[%d;0H", (fmc.cy - fmc.rowoff) + 1);
	wbWrite(&wb, buf, strlen(buf));

	write(STDOUT_FILENO, wb.b, wb.len);
	wbFree(&wb);
}

void init()
{
	fmc.cy = 0;
	fmc.rowoff = 0;
	fmc.numrows = 0;
	fmc.msgtime = 0;
	fmc.row = NULL;
	fmc.dirname = NULL;
	fmc.message[0] = '\0';

	if (getScreenSize(&fmc.rows, &fmc.cols) == -1)
		die(itoa(__LINE__, NULL));

	fmc.rows -= 2;
	signal(SIGWINCH, handleSigWinCh);
}

int main(int argc, char **argv)
{
	int c;

	while ((c = getopt(argc, argv, "nthvip")) != -1)
	{
		switch (c)
		{
		case 'n':
			fmc.exts.numbers = 1;
			break;
		case 't':
			fmc.exts.tildes = 1;
			break;
		case 'i':
			fmc.exts.info = 1;
			break;
		case 'p':
			fmc.exts.print = 1;
			break;
		case 'h':
			showHelp();
			break;
		case 'v':
			showVersion();
			break;
		}
	}

	enableRawMode();
	init();

	char *dir = get_current_dir_name();

	if (!argv[optind])
	{
		openDir(dir);
	}
	else if (strncmp(dir, argv[optind], 1) == 0)
	{
		openDir(argv[optind]);
	}
	else
	{
		char buf[strlen(dir) + strlen(argv[optind])];
		sprintf(buf, "%s/%s", dir, argv[optind]);
		openDir(buf);
	}

	free(dir);

	setMessage("Arrows | [R]ename | [*Ctrl][C]reate | [D]elete");

	while (1)
	{
		refreshScreen();
		processKey();
	}

	return 0;
}
