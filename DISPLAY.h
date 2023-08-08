#ifndef DISPLAY_h
#define DISPLAY_h

  void ConfigDisplay(void); //настройка дисплея (init)
  void ClsDisplay(void);//очистка
  void DisplayYX(int y, int x, char *str);//вывод
  int as();
  void Display(char *str);//4 bit print
  void write4bits(int value);
  long write(int value);
#endif