// exits.c
// 列出当前房间所有出口及其目标房间名，输出特殊格式供客户端解析。
// 输出格式：##EXITS##方向:房间名,方向:房间名,...##
// 例如：##EXITS##north:嘉兴城,south:土路,east:南湖##
inherit F_CLEAN_UP;

#include <room.h>
#include <ansi.h>

void create() { seteuid(getuid()); }

int main(object me, string arg)
{
    object env, room;
    mapping exits;
    string *dirs, *parts, dest, name;
    int i;

    env = environment(me);
    if (!objectp(env))
    {
        write("你的四周灰蒙蒙地一片，什么也看不到。\n");
        return 1;
    }

    if (!mapp(exits = env->query("exits")) || !sizeof(exits))
    {
        write("##EXITS####\n");
        return 1;
    }

    dirs = keys(exits);

    // 过滤掉已关闭的门
    for (i = 0; i < sizeof(dirs); i++)
        if ((int)env->query_door(dirs[i], "status") & DOOR_CLOSED)
            dirs[i] = 0;
    dirs -= ({ 0 });

    parts = ({});
    for (i = 0; i < sizeof(dirs); i++)
    {
        dest = exits[dirs[i]];
        if (!stringp(dest)) continue;

        // 尝试加载目标房间获取短描述
        if (!(room = find_object(dest)))
            catch(room = load_object(dest));

        if (objectp(room))
            name = (string)room->query("short");
        else
            name = "";

        // clean_color: 去掉 ANSI 颜色码
        if (stringp(name) && strlen(name) > 0)
        {
            name = replace_string(name, "\x1b[1;30m", "");
            name = replace_string(name, "\x1b[1;31m", "");
            name = replace_string(name, "\x1b[1;32m", "");
            name = replace_string(name, "\x1b[1;33m", "");
            name = replace_string(name, "\x1b[1;34m", "");
            name = replace_string(name, "\x1b[1;35m", "");
            name = replace_string(name, "\x1b[1;36m", "");
            name = replace_string(name, "\x1b[1;37m", "");
            name = replace_string(name, "\x1b[0;30m", "");
            name = replace_string(name, "\x1b[0;31m", "");
            name = replace_string(name, "\x1b[0;32m", "");
            name = replace_string(name, "\x1b[0;33m", "");
            name = replace_string(name, "\x1b[0;34m", "");
            name = replace_string(name, "\x1b[0;35m", "");
            name = replace_string(name, "\x1b[0;36m", "");
            name = replace_string(name, "\x1b[0;37m", "");
            name = replace_string(name, "\x1b[0m",    "");
            name = replace_string(name, "\x1b[m",     "");
        }

        if (!stringp(name) || strlen(name) == 0)
            name = dirs[i];

        parts += ({ dirs[i] + ":" + name });
    }

    write("##EXITS##" + implode(parts, ",") + "##\n");
    return 1;
}

int help(object me)
{
    write(@HELP
指令格式: exits

    列出当前房间所有可走的出口及其目标房间名称。
    输出格式供客户端软件解析使用。

HELP
    );
    return 1;
}
