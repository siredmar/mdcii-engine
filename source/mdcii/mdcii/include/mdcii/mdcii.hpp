#ifndef _MDCII_H_
#define _MDCII_H_
class Mdcii
{
public:
  Mdcii(int screen_width, int screen_height, bool fullscreen, int rate, const std::string& files_path);

private:
  static Uint32 timer_callback(Uint32 interval, void* param);
};

#endif