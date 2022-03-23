/*
 * =====================================================================================
 *
 *       Filename:  ChatBot.h
 *
 *    Description:  聊天机器人，针对DingTalk, telegram进行设置
 *
 *        Version:  1.0
 *        Created:  2020年07月21日 14时14分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once

#include "BaseCode.h"

class CChatBot
{
public:
    CChatBot();
    ~CChatBot();

    void setDingtalkToken(const CMyString &strToken, const CMyString &strKey);
    /* 发送钉钉文本消息
     * 参数： strText   文本消息
     *        atMobiles 需要@的用户手机列表，多个手机之间以','分隔; 空表示不需要at
     * 返回值： 0       发送成功
     *         -1000    http初始化失败
     *         -1001    无token
     *         -1003    KEY加密失败
     *         -1005    返回的数据内容出错
     *         >-1000   curl错误，参见LogErr描述
     *         <-300000 dingtalk的错误
     */
    int32_t dingtalk(const CMyString &content, const CMyString &atMobiles);

    int32_t dingtalk_link(const CMyString &title, const CMyString &content, const CMyString &picUrl, const CMyString &contentUrl);
    /* Markdown 格式：
     *      标题
     *      # 一级标题
     *      ## 二级标题
     *      ### 三级标题
     *      #### 四级标题
     *      ##### 五级标题
     *      ###### 六级标题
     *
     *      引用
     *      > A man who stands for nothing will fall for anything.
     *
     *      文字加粗、斜体
     *      **bold**
     *      *italic*
     *
     *      链接
     *      [this is a link](http://name.com)
     *
     *      图片
     *      ![](http://name.com/pic.jpg)
     *
     *      无序列表
     *      - item1
     *      - item2
     *
     *      有序列表
     *      1. item1
     *      2. item2
     */
    int32_t dingtalk_markdown(const CMyString &title, const CMyString &content, const CMyString &atMobiles);

    void setTelegramToken(const CMyString &strToken);
    /* 发送telegram文本消息
     * 参数：   chatId   聊天对象ID，正值为私聊，负值为群聊
     *                   注意， chatId 不能用 @channelusername 格式发送，测试过不行的。
     *          strText  文本消息
     * 返回值： 0       发送成功
     *         -1000    http初始化失败
     *         -1001    无token
     *         -1005    返回的数据内容出错
     *         >-1000   curl错误，参见LogErr描述
     *         <-300000 telegram错误，-300000是额外增加的值
     */
    int32_t telegram(const CMyString &chatId, const CMyString &strText, const CMyString &atUsers);

    /* parse_mode:  MarkdownV2  HTML  Markdown
     * MarkdownV2 =>
     *      *bold \*text*
     *      _italic \*text_
     *      __underline__
     *      ~strikethrough~
     *      *bold _italic bold ~italic bold strikethrough~ __underline italic bold___ bold*
     *      [inline URL](http://www.example.com/)
     *      [inline mention of a user](tg://user?id=123456789)
     *      `inline fixed-width code`
     *      ```
     *      pre-formatted fixed-width code block
     *      ```
     *      ```python
     *      pre-formatted fixed-width code block written in the Python programming language
     *      ```
     *  Please note:
     *      1. Any character with code between 1 and 126 inclusively can be escaped anywhere with a preceding '\' character, in which case it is treated as an ordinary character and not a part of the markup.
     *      2. Inside pre and code entities, all '`' and '\' characters must be escaped with a preceding '\' character.
     *      3. Inside (...) part of inline link definition, all ')' and '\' must be escaped with a preceding '\' character.
     *      4. In all other places characters '_', '*', '[', ']', '(', ')', '~', '`', '>', '#', '+', '-', '=', '|', '{', '}', '.', '!' must be escaped with the preceding character '\'.
     *      5. In case of ambiguity between italic and underline entities __ is always greadily treated from left to right as beginning or end of underline entity, so instead of ___italic underline___ use ___italic underline_\r__, where \r is a character with code 13, which will be ignored.'`
     *
     *
     * HTML =>
     *      <b>bold</b>, <strong>bold</strong>
     *      <i>italic</i>, <em>italic</em>
     *      <u>underline</u>, <ins>underline</ins>
     *      <s>strikethrough</s>, <strike>strikethrough</strike>, <del>strikethrough</del>
     *      <b>bold <i>italic bold <s>italic bold strikethrough</s> <u>underline italic bold</u></i> bold</b>
     *      <a href="http://www.example.com/">inline URL</a>
     *      <a href="tg://user?id=123456789">inline mention of a user</a>
     *      <code>inline fixed-width code</code>
     *      <pre>pre-formatted fixed-width code block</pre>
     *      <pre><code class="language-python">pre-formatted fixed-width code block written in the Python programming language</code></pre>
     * Please note:
     *      1. Only the tags mentioned above are currently supported.
     *      2. All <, > and & symbols that are not a part of a tag or an HTML entity must be replaced with the corresponding HTML entities (< with &lt;, > with &gt; and & with &amp;).
     *      3. All numerical HTML entities are supported.
     *      4. The API currently supports only the following named HTML entities: &lt;, &gt;, &amp; and &quot;.
     *      5. Use nested pre and code tags, to define programming language for pre entity.
     *      6. Programming language can't be specified for standalone code tags.
     *
     *
     * Markdown =>
     *      *bold text*
     *      _italic text_
     *      [inline URL](http://www.example.com/)
     *      [inline mention of a user](tg://user?id=123456789)
     *      `inline fixed-width code`
     *      ```
     *      pre-formatted fixed-width code block
     *      ```
     *      ```python
     *      pre-formatted fixed-width code block written in the Python programming language
     *      ```
     * Please note:
     *      1. Entities must not be nested, use parse mode MarkdownV2 instead.
     *      2. There is no way to specify underline and strikethrough entities, use parse mode MarkdownV2 instead.
     *      3. To escape characters '_', '*', '`', '[' outside of an entity, prepend the characters '\' before them.
     *      4. Escaping inside entities is not allowed, so entity must be closed first and reopened again: use _snake_\__case_ for italic snake_case and *2*\**2=4* for bold 2*2=4.]`'
     */
    int32_t telegram_ex(const CMyString &chatId, const CMyString &strText, const CMyString &atUsers, const CMyString &parseMode);

private:
    CMyString   m_telegram_token;
    CMyString   m_dingtalk_token;
    CMyString   m_dingtalk_key;
};

int32_t dingtalk_sendmsg(const CMyString &strText, const CMyString &strToken, 
        const CMyString &strKey, const CMyString &stAtMobiles);

int32_t telegram_sendmsg(const CMyString &chatId, const CMyString &strText, 
        const CMyString &strToken, const CMyString &atUsers);


