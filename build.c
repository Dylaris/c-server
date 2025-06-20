#define ZST_IMPLEMENTATION
#include "zst.h"

#define CC      "gcc"
#define SRC_DIR "src/"
#define INC_DIR "src/include/"
#define TARGET  "main"

static zst_forger_t forger;

static void compile_one_file(const char *obj, const char *src)
{
    zst_cmd_t cmd;
    zst_cmd_init(&cmd);
    zst_cmd_append_arg(&cmd, CC, "-c", "-Wall", "-Wextra", 
            "-I", INC_DIR, "-o", obj, src);
    zst_forger_append_cmd(&forger, &cmd);
}

static void compile(void)
{
    /* Compile */
    for (size_t i = 0; i < forger.srcs.count; i++) {
        zst_string_t *src = zst_dyna_get(&forger.srcs, i);
        zst_string_t *obj = zst_dyna_get(&forger.objs, i);
        compile_one_file(obj->base, src->base);
    }

    /* Link */
    zst_cmd_t cmd;
    zst_cmd_init(&cmd);
    zst_cmd_append_arg(&cmd, CC, "-Wall", "-Wextra",
            "-I", INC_DIR, "-o", TARGET);
    for (size_t i = 0; i < forger.objs.count; i++) {
        zst_string_t *obj = zst_dyna_get(&forger.objs, i);
        zst_cmd_append_arg(&cmd, obj->base);
    }
    zst_forger_append_cmd(&forger, &cmd);

    zst_forger_run_sync(&forger);
}

static void clean(void)
{
    zst_fs_remove_all(&forger.objs);
    zst_fs_remove(TARGET);
}

static void test(void)
{
}

static void define_flags(zst_cmdline_t *cmdl)
{
    zst_cmdline_define_flag(cmdl, FLAG_NO_ARG, "h", "Print this information");
    zst_cmdline_define_flag(cmdl, FLAG_NO_ARG, "c", "Compile all source files");
    zst_cmdline_define_flag(cmdl, FLAG_NO_ARG, "t", "Run tests");
    zst_cmdline_define_flag(cmdl, FLAG_NO_ARG, "cl", "Clean all generated files");
}

static void callback(zst_cmdline_t *cmdl)
{
    bool flag_h  = zst_cmdline_isuse(cmdl, "h");
    bool flag_c  = zst_cmdline_isuse(cmdl, "c");
    bool flag_t  = zst_cmdline_isuse(cmdl, "t");
    bool flag_cl = zst_cmdline_isuse(cmdl, "cl");

    if (flag_h) zst_cmdline_usage(cmdl);
    if (flag_t) test();
    if (flag_c) compile();
    if (flag_cl) clean();
}

int main(int argc, char **argv)
{
    zst_forger_update_self(CC, argc, argv);

    zst_forger_init(&forger, TARGET, SRC_DIR, true);

    zst_cmdline_t cmdl;
    zst_cmdline_init(&cmdl);
    define_flags(&cmdl);

    zst_cmdline_parse(&cmdl, argc, argv);

    callback(&cmdl);

    zst_cmdline_free(&cmdl);
    zst_forger_free(&forger);
    return 0;
}
