#ifndef __LST_H__
#define __LST_H__

#include <time.h>

#define LST_VERSION "1.1.1"
#define WBUF_INIT {NULL, 0}
#define CTRL_KEY(k) ((k)&0x1f)
#define TOK_BUFSIZE 64
#define T_FILE 1
#define T_DIR 2
#define REFRESH 3
#define FORWARD 1
#define BACK 2

enum keys
{
	BACKSPACE = 127,
	ARROW_LEFT = 1000,
	ARROW_RIGHT,
	ARROW_UP,
	ARROW_DOWN,
	DEL_KEY,
	PAGE_UP,
	PAGE_DOWN
};

typedef struct fmrow
{
	int size;
	char *chars;
} fmrow;

struct wbuf
{
	char *b;
	int len;
};

struct extensions
{
	int numbers;
	int tildes;
	int print;
	int info;
};

struct fmConfig
{
	int rows;
	int cols;
	int cy;
	int rowoff;
	int numrows;
	char *dirname;
	char message[80];
	struct termios std;
	struct extensions exts;
	fmrow *row;
	time_t msgtime;
} fmc;

void init(void);
void scroll(void);
void refreshScreen(void);

void enableRawMode(void);
void disableRawMode(void);
void handleSigWinCh(int signo);

int readKey(void);
void processKey(void);
void moveCursor(int key);

void setMessage(const char *fmt, ...);
char *prompt(char *prompt);

void openFile(void);
void openDir(char *dirname);
void changeDir(int op);

void deleteEntry(void);
void renameEntry(char *newname);
void createEntry(char *entry, int type);

char *truncateDir(char *_dir);
void appendPath(char *path, char *buf, int bufsize);
void highlightLine(struct wbuf *wb, const char *s, int len);
void insertRow(char *s, size_t len);

void drawRows(struct wbuf *wb);
void drawStatusBar(struct wbuf *wb);
void drawMessageBar(struct wbuf *wb);

char *getCurrentTime(void);
int getFileSize(char *filename);
int getCursorPos(int *rows, int *cols);
int getScreenSize(int *rows, int *cols);
int getRowNoLen(char *buf, int size, int lineNo);

void wbWrite(struct wbuf *wb, const char *s, int len);
void wbFree(struct wbuf *wb);

void die(char *s);
void freeRows(void);
void onExit(void);

char *itoa(int num, char *buf);

#endif // __LST_H__