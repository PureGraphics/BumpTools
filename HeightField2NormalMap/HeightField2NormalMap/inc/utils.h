#ifndef utils_h
#define utils_h

#ifdef __cplusplus
extern "C" {
#endif

wchar_t * char2wchar_t(const char *str);
char *    wchar_t2char(const wchar_t *str);

#ifdef __cplusplus
}
#endif

#endif