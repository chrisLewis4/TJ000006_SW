git describe --tags --always --dirty or git rev-parse --short HEAD 
echo #define FW_VERSION \"$(git describe --tags --always --dirty)\" > version.h