// info.c
// 查询目标对象的可用交互动作，输出特殊格式供客户端解析。
// 输出格式：##INFO##对象id|cmd:标签,cmd:标签,...##
// 例如：##INFO##wuya|look:查看,ask:问话,kill:攻击##
inherit F_CLEAN_UP;

#include <ansi.h>

void create() { seteuid(getuid()); }

int main(object me, string arg)
{
    object ob, env;
    string *parts;
    string obj_id;

    if (!arg)
        return notify_fail("用法：info <对象>\n");

    env = environment(me);
    ob = present(arg, env);
    if (!ob) ob = present(arg, me);
    if (!ob)
        return notify_fail("这里没有" + arg + "。\n");

    // 获取对象 id（取第一个）
    if (arrayp(ob->parse_command_id_list()) && sizeof(ob->parse_command_id_list()) > 0)
        obj_id = (ob->parse_command_id_list())[0];
    else
        obj_id = arg;

    parts = ({ "look:查看" });

    if (living(ob) && ob != me)
    {
        if (ob->query("can_speak"))
            parts += ({ "ask:问话" });
        parts += ({ "kill:攻击" });
        parts += ({ "give:赠予" });
        parts += ({ "steal:偷窃" });
        if (ob->query("is_vendor"))
            parts += ({ "buy:购买" });
        if (userp(ob))
            parts += ({ "jiebai:结拜" });
    }
    else if (!living(ob))
    {
        parts += ({ "get:拾取" });
    }

    write("##INFO##" + obj_id + "|" + implode(parts, ",") + "##\n");
    return 1;
}

int help(object me)
{
    write(@HELP
指令格式: info <对象>

    查询目标对象（NPC、玩家或物品）支持哪些交互动作。
    输出格式供客户端软件解析使用。

HELP
    );
    return 1;
}
