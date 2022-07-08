#include "types.h"
#include "stat.h"
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0)
    putc(fd, buf[i]);
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, const char *fmt, ...)
{
  char *s;
  int c, i, state;
  uint *ap;

  // 0 print directly; % print in format; \e change color
  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if((c != '%') && (c != '\e')){
        putc(fd, c);
      } else{
        state = c;
      }
    } else if(state == '%'){
      if(c == 'd'){
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
        s = (char*)*ap;
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
        putc(fd, c);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
    } else if(state == '\e'){
      s = "\e[38;5;15m";
      if(c == '0') s = "\e[38;5;8m";
      if(c == '1') s = "\e[38;5;9m";
      if(c == '2') s = "\e[38;5;10m";
      if(c == '3') s = "\e[38;5;11m";
      if(c == '4') s = "\e[38;5;12m";
      if(c == '5') s = "\e[38;5;13m";
      if(c == '6') s = "\e[38;5;14m";
      if(c == '7') s = "\e[38;5;15m";
      write(fd, s, strlen(s));
      state = 0;
    }
    s = "\e[38;5;15m";
    write(fd, s, strlen(s));
  }
  s = "\e[38;5;15m";
  write(fd, s, strlen(s));
}
