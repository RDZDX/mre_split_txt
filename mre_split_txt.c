#include "mre_split_txt.h"

#define MAX_PATH_LEN        100
#define MAX_EXT_LEN         5
#define MAX_COUNTER_LEN     5
#define MAX_TITLE_LEN       42
#define MAX_LINE_SIZE       512

VMBOOL trigeris = VM_FALSE;
VMWCHAR title_name[MAX_TITLE_LEN] = {0};
VMINT check_selector = -1;
VMINT byte_lenght = 0;
VMWCHAR path[MAX_PATH_LEN];
VMINT autonum_num = 1;

void vm_main(void) {

    vm_reg_sysevt_callback(handle_sysevt);
    vm_ascii_to_ucs2(title_name, (strlen("Input size in bytes:") + 1) * 2, "Input size in bytes:");
    vm_input_set_editor_title(title_name);
    vm_input_text3(NULL, 1200, 4, save_text);
}

void handle_sysevt(VMINT message, VMINT param) {

    switch (message) {
        case VM_MSG_CREATE:
        case VM_MSG_ACTIVE:
            break;
        case VM_MSG_PAINT:
            if (trigeris == VM_TRUE) {
                vm_exit_app();
            }
            break;
        case VM_MSG_INACTIVE:
        case VM_MSG_QUIT:
            break;
    }
}

void save_text(VMINT state, VMWSTR text) {

    VMCHAR tmp[32] = {0};
    VMINT length = vm_wstrlen(text);
    VMINT value;

    if (state == VM_INPUT_OK && length > 0) {
        vm_ucs2_to_ascii(tmp, (length + 1), text);

        if (safe_str_to_int(tmp, &value)) {
            byte_lenght = value;
        } else {
            vm_exit_app();
        }

        check_selector = vm_selector_run(0, 0, job);
        if (check_selector == 0) {
            trigeris = VM_TRUE;
        }
    } else {
        vm_exit_app();
    }
}

VMINT job(VMWSTR filepath, VMINT wlen) {

    VMFILE f_read = -1, f_write = -1;
    VMUINT file_size = 0, nwrite = 0;
    VMCHAR *line_buffer = NULL;
    VMINT result = -1;
    VMINT current_size = 0;
    VMINT pos = 0;
    VMWCHAR tmp_path[MAX_PATH_LEN];

    vm_get_path(filepath, path);

    f_read = vm_file_open(filepath, MODE_READ, TRUE);
    if (f_read < 0) return -1;

    if (vm_file_getfilesize(f_read, &file_size) < 0) {
        vm_file_close(f_read);
        return -1;
    }

    line_buffer = (VMCHAR *)vm_malloc(MAX_LINE_SIZE);
    if (!line_buffer) {
        vm_file_close(f_read);
        return -1;
    }

    if (autonum_num > 50) { //Fuse remove it !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
       //vm_msgbox_activate(alert_title, alert_message, VM_MSGBOX_TYPE_OK, alert_callback);
       result = -1;
       goto cleanup;
    }

    auto_counter_path(tmp_path, filepath);

    f_write = vm_file_open(tmp_path, MODE_CREATE_ALWAYS_WRITE, TRUE);
    if (f_write < 0) goto cleanup;

    while (1) {
        VMCHAR ch;
        VMUINT read_len;

        if (vm_file_read(f_read, &ch, 1, &read_len) < 0 || read_len == 0) {
            if (pos > 0) {
                vm_file_write(f_write, line_buffer, pos, &nwrite);
            }
            break;
        }

        line_buffer[pos++] = ch;

        if (ch == '\n' || pos >= MAX_LINE_SIZE - 1) {
            if (current_size + pos > byte_lenght) {
                vm_file_close(f_write);
                current_size = 0;

                if (autonum_num > 50) { //Fuse remove it !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                   result = -1;
                   goto cleanup;
                }

                auto_counter_path(tmp_path, filepath);

                f_write = vm_file_open(tmp_path, MODE_CREATE_ALWAYS_WRITE, TRUE);
                if (f_write < 0) goto cleanup;
            }

            vm_file_write(f_write, line_buffer, pos, &nwrite);
            current_size += pos;
            pos = 0;
        }
    }

    result = 0;

cleanup:
    if (line_buffer) vm_free(line_buffer);
    if (f_read >= 0) vm_file_close(f_read);
    if (f_write >= 0) vm_file_close(f_write);

    return result;
}

VMBOOL safe_str_to_int(const char *str, int *outValue) {

    if (!str || *str == '\0') return VM_FALSE;

    char *endptr;
    long val = strtol(str, &endptr, 10);
    if (*endptr != '\0') return VM_FALSE;
    if (val < INT_MIN || val > INT_MAX) return VM_FALSE;

    *outValue = (int)val;
    return VM_TRUE;
}

void auto_counter_path(VMWSTR result, VMWSTR path) {

    VMCHAR cpath[MAX_PATH_LEN] = {0};
    VMCHAR cextn[MAX_EXT_LEN] = {0};
    VMCHAR cpath_plus[MAX_PATH_LEN] = {0};
    VMCHAR counter_text[MAX_COUNTER_LEN] = {0};
    VMCHAR cpathn[MAX_PATH_LEN] = {0};
    VMWSTR newpathw = (VMWSTR)vm_malloc(MAX_PATH_LEN * 2);

    if (!newpathw) return;

    vm_ucs2_to_ascii(cpath, (vm_wstrlen(path) + 1), path);
    extract_end_text(cextn, cpath, '.');

    int base_len = strlen(cpath) - strlen(cextn) - 1;
    if (base_len < 0 || base_len >= MAX_PATH_LEN) {
        vm_free(newpathw);
        return;
    }

    strncpy(cpath_plus, cpath, base_len);
    cpath_plus[base_len] = '\0';

    sprintf(counter_text, "%03d", autonum_num);
    strcat(cpath_plus, counter_text);
    strcat(cpath_plus, ".");
    strcat(cpath_plus, cextn);

    strcpy(cpathn, cpath_plus);
    vm_ascii_to_ucs2(newpathw, (strlen(cpathn) + 1) * 2, cpathn);

    autonum_num++;

    vm_wstrcpy(result, newpathw);
    vm_free(newpathw);
}

void extract_end_text(char *result_data, const char *inp_data, char separator) {

    int i = 0;
    int last_sep_pos = -1;

    while (inp_data[i] != '\0') {
        if (inp_data[i] == separator) {
            last_sep_pos = i;
        }
        i++;
    }

    if (last_sep_pos != -1) {
        i = 0;
        int j = last_sep_pos + 1;
        while (inp_data[j] != '\0') {
            result_data[i++] = inp_data[j++];
        }
        result_data[i] = '\0';
    } else {
        result_data[0] = '\0';  // No separator found
    }
}