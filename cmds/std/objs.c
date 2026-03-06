// objs.c
// 列出当前房间所有可交互对象，输出特殊格式供客户端解析。
// 输出格式：##OBJECTS##id:显示名:类型,id:显示名:类型,...##
// 类型：npc=活体NPC, player=玩家, item=物品
// 例如：##OBJECTS##wuya:四只乌鸦:npc,board:公告版:item##
inherit F_CLEAN_UP;

#include <ansi.h>

void create() { seteuid(getuid()); }

// 去除ANSI颜色码
string clean_color(string s)
{
    if (!s) return "";
    s = regexp(s, "\x1b\[[^m]*m", "");
    return s;
}

int main(object me, string arg)
{
    object env, *inv;
    string *parts, obj_id, obj_short;
    string obj_type;
    int i;

    env = environment(me);
    if (!objectp(env))
    {
        write("##OBJECTS####\n");
        return 1;
    }

    inv = all_inventory(env);
    parts = ({});

    for (i = 0; i < sizeof(inv); i++)
    {
        object ob = inv[i];

        // 跳过玩家自身
        if (ob == me) continue;
        // 跳过隐身对象
        if (ob->query_temp("apply/yinshen")) continue;
        // 跳过设置了 no_shown 的对象
        if (ob->query("no_shown")) continue;
        // 跳过不可见对象
        if (!me->visible(ob)) continue;

        // 获取对象id（取第一个parse_command_id_list）
        if (arrayp(ob->parse_command_id_list()) && sizeof(ob->parse_command_id_list()) > 0)
            obj_id = (ob->parse_command_id_list())[0];
        else if (stringp(ob->query("id")))
            obj_id = ob->query("id");
        else
            continue;

        // 获取显示名（short描述，去颜色）
        obj_short = clean_color((string)ob->short());
        if (!stringp(obj_short) || strlen(obj_short) == 0)
            obj_short = obj_id;

        // 判断类型
        if (userp(ob))
            obj_type = "player";
        else if (living(ob))
            obj_type = "npc";
        else
            obj_type = "item";

        parts += ({ obj_id + ":" + obj_short + ":" + obj_type });
    }

    write("##OBJECTS##" + implode(parts, ",") + "##\n");
    return 1;
}

int help(object me)
{
    write(@HELP
指令格式: objs

    列出当前房间内所有可交互对象（NPC、玩家、物品）。
    输出格式供客户端软件解析使用。
    格式：##OBJECTS##id:显示名:类型,...##

HELP
    );
    return 1;
}
