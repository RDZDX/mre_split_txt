#ifndef _VRE_APP_WIZARDTEMPLATE_
#define	_VRE_APP_WIZARDTEMPLATE_

#include "vmio.h"
#include "string.h"
#include "stdint.h"
#include "vmdes.h"
#include "vmchset.h"
#include "vmstdlib.h"
#include <limits.h>

void handle_sysevt(VMINT message, VMINT param);
void save_text(VMINT state, VMWSTR text);
VMINT job(VMWSTR filepath, VMINT wlen);
VMBOOL safe_str_to_int(const char *str, int *outValue);
void auto_counter_path(VMWSTR result, VMWSTR path);
void extract_end_text(char *result_data, const char *inp_data, char separator);

#endif

