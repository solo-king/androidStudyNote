#define		LOG_TAG		"UsrStringTranslate"
#include <cutils/log.h>

#include "../include/usr_string_translate.h"

int string16_to_string8(unsigned short *string16, char*string8, int length)
{
	int count = 0;
	
	if((string16 == NULL)||(length <= 0)){
		ALOGE("arg is err!!" );
		return -1;
	}
	
	for(count = 0; count < length; ++count)
		*(string8+count) = *(string16+count);
	*(string8+count) = '\0';
	
	return 0;
}
