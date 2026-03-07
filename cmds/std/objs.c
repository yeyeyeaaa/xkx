// objs.c
// 列出当前房间所有可交互对象，输出特殊格式供客户端解析。
// 输出格式：##OBJECTS##id:显示名:类型,id:显示名:类型,...##
// 类型：npc=活体NPC, player=玩家, item=物品
// 例如：##OBJECTS##wuya:四只乌鸦:npc,board:公告版:item##
inherit F_CLEAN_UP;

#include <ansi.h>

void create() { seteuid(getuid()); }

// 去除ANSI颜色码（用 replace_string 逐一替换，兼容此 FluffOS 版本）
string clean_color(string s)
{
    if (!s) return "";
    s = replace_string(s, BLK,   "");
    s = replace_string(s, RED,   "");
    s = replace_string(s, GRN,   "");
    s = replace_string(s, YEL,   "");
    s = replace_string(s, BLU,   "");
    s = replace_string(s, MAG,   "");
    s = replace_string(s, CYN,   "");
    s = replace_string(s, WHT,   "");
    s = replace_string(s, HIR,   "");
    s = replace_string(s, HIG,   "");
    s = replace_string(s, HIY,   "");
    s = replace_string(s, HIB,   "");
    s = replace_string(s, HIM,   "");
    s = replace_string(s, HIC,   "");
    s = replace_string(s, HIW,   "");
    s = replace_string(s, HIK,   "");
    s = replace_string(s, NOR,   "");
    s = replace_string(s, BOLD,  "");
    s = replace_string(s, BLINK, "");
    s = replace_string(s, U,     "");
    return s;
}

// 从长名中提取短名：
// 去掉括号部分 "(Xiao er)"，去掉书名号绰号 "「玉面蛇心」"，取最后剩下的中文名
// 例：
//   "店小二(Xiao er)"            → "店小二"
//   "白驼山少庄主「玉面蛇心」欧阳克(Ouyang ke)" → "欧阳克"
//   "四只乌鸦"                    → "四只乌鸦"（不变）
string extract_short_name(string s)
{
    int i, lpar, rpar, lbook, rbook, len;
    string result;

    if (!stringp(s) || strlen(s) == 0) return s;

    // 1. 去掉括号 (...) 及其内容
    //    找最后一个 '(' 之前的部分（英文括号）
    lpar = -1;
    len = strlen(s);
    for (i = len - 1; i >= 0; i--)
    {
        if (s[i] == '(')
        {
            lpar = i;
            break;
        }
    }
    if (lpar > 0)
        s = s[0..lpar - 1];

    // 2. 去掉书名号绰号「...」及其前面的称谓（取「」之后的部分作为名字）
    //    如果有「...」，取最后一个 」 之后的内容作为真名
    //    UTF-8下 「=\xe3\x80\x8a 或 \xe3\x80\x8e，」=对应闭合
    //    LPC 中直接按字节搜索更可靠，用 strsrch 查找
    i = strsrch(s, "」", -1);  // 从右往左找最后一个 」
    if (i > 0 && i < strlen(s) - 1)
        s = s[i + 1..];         // 取 」 后面的部分

    // 3. 去掉前后空白
    s = trim(s);

    if (!stringp(s) || strlen(s) == 0)
        return s;

    return s;
}

int main(object me, string arg)
{
    object env, *inv;
    string *parts, obj_id, obj_short, obj_name;
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

        // 获取完整显示名（short描述，去颜色）
        obj_short = clean_color((string)ob->short());
        if (!stringp(obj_short) || strlen(obj_short) == 0)
            obj_short = obj_id;

        // 提取短名（去掉括号英文名和绰号，供左侧列显示）
        obj_name = extract_short_name(obj_short);
        if (!stringp(obj_name) || strlen(obj_name) == 0)
            obj_name = obj_short;

        // 判断类型
        if (userp(ob))
            obj_type = "player";
        else if (living(ob))
            obj_type = "npc";
        else
            obj_type = "item";

        // 协议格式：id:短名:完整名:类型
        // 客户端左侧列用短名，浮层标题用完整名
        parts += ({ obj_id + ":" + obj_name + ":" + obj_short + ":" + obj_type });
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
