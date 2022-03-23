#-- coding:UTF-8 --
import MySQLdb
import os
import datetime
import xml.dom.minidom as minidom
import base64

def is_single_comment(str):
    if str[0:2] == '//' or str[0] == '#' or str[0:2]=='++':
        return True
    if ('/*' in str) and ('*/' in str):
        return True
    return False

def is_multi_comment(str):
    if '/*' in str:
        return True
    return False

def is_multi_comment_end(str):
    if '*/' in str:
        return True
    return False

def svn_linecount(repo, rev, user, passwd):
    cmd='svn log '+repo+' --diff -r '+str(rev)+' --username='+user+' --password='+passwd
#    print(cmd)
    results = os.popen(cmd)
    line_count = 0
    bComment = False
    for line in results:
        line = line.strip()
        if line is None or len(line)<=1:
            continue
## 判断是否注释行
        if (bComment):
            if is_multi_comment_end(line):
                bComment = False
                continue
            continue

        if line[0] != '+':
            continue
        line = line[1:].strip()
        if is_single_comment(line):
            continue
        if is_multi_comment(line):
            bComment = True
            continue
#        print line
        line_count = line_count + 1
    return line_count

def decode_passwd(cipher):
    text = base64.b64decode(cipher)
    clear = text[2:5]+text[7:len(text)]
    return clear

def svn_max_rev(repo, user, passwd):
    cmd = "svn info "+repo+" --username="+user+" --password="+passwd+" | grep '最后修改的版本:' | awk -F ':' '{print $2}' "
    print(cmd)
    head_rev = os.popen(cmd)
    res = head_rev.read()
    rev = "0"
    for line in res.splitlines():
        rev = line.strip()
#        print rev
    return int(rev)

def svn_from_xml(xml):
# 从XML中解析出可用的元素    
    dom = minidom.parse(xml)
    root = dom.documentElement
    repos = root.getElementsByTagName("svn")
    repo_list = []
    for repo in repos:
        name = repo.getAttribute("name").encode('utf-8')
        url = repo.getAttribute("url").encode('utf-8')
        user = repo.getAttribute("user").encode('utf-8')
        passwd = repo.getAttribute("passwd").encode('utf-8')
        passwd = decode_passwd(passwd)
        start_rev = 1
        if repo.hasAttribute("start_rev"):
            start_rev = int(repo.getAttribute("start_rev"))
        svn_head_rev = svn_max_rev(url, user, passwd)
        if svn_head_rev>0:
            repo_tup = (name, url, user, passwd, svn_head_rev, start_rev)
            repo_list.append(repo_tup)
    return repo_list

def mysql_conninfo(xml):
# 从XML中获得mysql的连接信息    
    dom = minidom.parse(xml)
    root = dom.documentElement
    mysql_list = root.getElementsByTagName("dbms_mysql")
    for mysql in mysql_list:
        host = mysql.getAttribute("host").encode('utf-8');
        db = mysql.getAttribute("db").encode('utf-8');
        user = mysql.getAttribute("user").encode('utf-8');
        passwd = mysql.getAttribute("passwd").encode('utf-8')
        passwd = decode_passwd(passwd)
        return (host, user, passwd, db)
    return None 

def mysql_get_rev(conn, repo_name):
    rev = 0
    sql = "SELECT IFNULL(MIN(`rev`),-1) FROM `svnstat` WHERE `name`='%s' AND `line`=-1;" % (repo_name)
    try:
        cursor = conn.cursor()
        cursor.execute(sql)
        results = cursor.fetchall()
        for row in results:
            rev = row[0]
        cursor.close()
    except MySQLdb.Error, e:
        print "MySQL Error, err:%d, msg:%s" %(e.args[0], e.args[1])
        sys.exit(1)
    return rev

def mysql_update_rev(conn, repo, rev, lineCount):
    sql = "call pSvnStatLine('%s', %d, %d)" % (repo, rev, lineCount)
#    print(sql)
    try:
        cursor = conn.cursor()
        cursor.execute(sql)
        conn.commit()
        cursor.close()
    except MySQLdb.Error, e:
        print "MySQL Error, err:%d, msg:%s" % (e.args[0], e.args[1])
        sys.exit(1)
        
def main():
#    cnt = svn_diff('http://192.168.1.103:18080/svn/crazyflight_v3/trunk', 1316, 'auditor', 'D#Ffj6kH@P4eL')
#    print 'total line count %d' % cnt
#    xml_file = "../bin/config.xml"
    xml_file = "./config.xml"
    repo_list = svn_from_xml(xml_file)

    mysql_info = mysql_conninfo(xml_file)
    if mysql_info is None:
        print "not mysql connect info."
        sys.exit(1)
    conn = MySQLdb.connect(mysql_info[0], mysql_info[1], mysql_info[2], mysql_info[3], charset='utf8mb4')

    for repo in repo_list:
        repo_name = repo[0]
        url = repo[1]
        user = repo[2]
        passwd = repo[3]
        head_rev = repo[4]
        while True:
            rev = mysql_get_rev(conn, repo_name)
            if rev == -1:
                print "repo %s all rev has processed." % (repo_name)
                break
            if rev > head_rev:
                break 
            line_count = svn_linecount(url, rev, user, passwd)
            print 'repo %s, rev:%d, line:%d, head rev:%d' % (repo_name, rev, line_count, head_rev)
            mysql_update_rev(conn, repo_name, rev, line_count)
    conn.close()    

if __name__ == '__main__':
    main()

