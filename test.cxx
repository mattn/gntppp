#include "gntp.h"

using namespace CryptoPP;
using namespace CryptoPP::Weak1;

int main(void) {
  try {
    gntp client("my growl application", ""); // you should set password

    //client.regist("my-event");
    client.regist<DES, MD5>("my-event");

    //client.notify("my-event", "通知", "こんにちわ\r\n世界", "http://mattn.kaoriya.net/images/logo.png");
    client.notify<DES, SHA256>(
			"my-event",
			"通知",
			"こんにちわ\r\n世界",
			"http://mattn.kaoriya.net/images/logo.png",
			"http://mattn.kaoriya.net/");

    // NOTE: note that you should use SHA256 for AES. AES require long key size.
  } catch(const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
