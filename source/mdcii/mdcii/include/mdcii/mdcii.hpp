#ifndef _MDCII_H_
#define _MDCII_H_
class Mdcii
{
public:
  explicit Mdcii(int screen_width, int screen_height, bool fullscreen, const std::string& files_path);

private:
  static Uint32 TimerCallback(Uint32 interval, void* param);
};

#endif