#define ZST_IMPLEMENTATION
#include "zst.h"

#define CC       "gcc"
#define TEST_DIR "test/"
#define SRC_DIR  "src/"
#define LIBNAME  "libserv.a"

static void compile_test_files(void)
{
    zst_dyna_t files = zst_fs_match(TEST_DIR, "*.c");
    for (size_t i = 0; i < files.count; i++) {
        zst_string_t *file = zst_dyna_get(&files, i);
        zst_string_t exe = zst_string_replace(file->base, ".c", "");

        zst_cmd_t cmd;
        zst_cmd_init(&cmd);
        zst_cmd_append_arg(&cmd, CC, 
                           "-Wall", "-Wextra",
                           "-I", SRC_DIR,
                           "-o", exe.base, file->base,
                           "./src/"LIBNAME);
        zst_cmd_run(&cmd);

        zst_cmd_free(&cmd);
        zst_string_free(&exe);
    }
    zst_dyna_free(&files);
}

static void create_static_library(void)
{
    zst_fs_cd(SRC_DIR);

    zst_dyna_t files = zst_fs_match(".", "*.c");
    for (size_t i = 0; i < files.count; i++) {
        zst_string_t *file = zst_dyna_get(&files, i);

        zst_cmd_t cmd;
        zst_cmd_init(&cmd);
        zst_cmd_append_arg(&cmd, CC, 
                           "-Wall", "-Wextra",
                           "-c", file->base);
        zst_cmd_run(&cmd);

        zst_cmd_free(&cmd);
    }
    zst_dyna_free(&files);

    files = zst_fs_match(".", "*.o");
    zst_cmd_t cmd;
    zst_cmd_init(&cmd);

    zst_cmd_append_arg(&cmd, "ar", "r", LIBNAME);
    for (size_t i = 0; i < files.count; i++) {
        zst_string_t *file = zst_dyna_get(&files, i);
        zst_cmd_append_arg(&cmd, file->base);
    }

    zst_cmd_run(&cmd);

    zst_cmd_free(&cmd);
    zst_dyna_free(&files);

    zst_fs_cd("..");
}

int main(int argc, char **argv)
{
    zst_forger_update_self(CC, argc, argv);
    create_static_library();
    compile_test_files();
    return 0;
}
