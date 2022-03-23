#-- coding:UTF-8 --
import MySQLdb
import os
#from datetime import datetime
import datetime
import xml.dom.minidom as minidom
import base64

def utc2local(timestr):
    timestr = timestr.strip()
    time_list = timestr.split(' ')
    if len(time_list)>3 :
        date_str = time_list[0]
        time_str = time_list[1]
        tz_str = time_list[2]
        y,m,d = date_str.split('-')
        H,M,S = time_str.split(':')
#        tz = int(tz_str)/100
        d1 = datetime.datetime(int(y), int(m), int(d), int(H), int(M), int(S), 0)
#        d2 = datetime.timedelta(hours = tz)
#        d3 = d1+d2
#        return d3.strftime("%Y-%m-%d %H:%M:%S")
        return d1.strftime("%Y-%m-%d %H:%M:%S")
    else:
        return timestr

class svn_base_log:
    def __init__(self):
        self.revision=0
        self.author=""
        self.time=""
        self.message=""
    def setRevision(self, revision):
        self.revision = int(revision[1:-1])
    def setAuthor(self, author):
        self.author=author.strip()
    def setTime(self, time):
        self.time=utc2local(time)
    def setMessage(self, msg):
        if self.message=='':
            self.message = MySQLdb.escape_string(msg.strip())
        else:
            self.message += '\n'
            self.message += MySQLdb.escape_string(msg.strip())
    def getRevision(self):
        return self.revision
    def getAuthor(self):
        return self.author
    def getTime(self):
        return self.time
    def getMessage(self):
        return self.message
    def __str__(self):
        str = ("Revision:%d\nTime:%s\nAuthor:%s\nMessage:%s") % (self.revision, self.time, self.author, self.message)
        return str

def svn_log_split(repo, rev, user, passwd):
    cmd = "svn log -r " + str(rev) + " " + repo + " --username="+user+" --password="+passwd
#    print(cmd)
    svn_logs = os.popen(cmd)
    res = svn_logs.read()

    log = svn_base_log()
    for line in res.splitlines():
        if line is None or (len(line)<1):
            continue
        if '------' in line:
            continue
        if line.count('|') >=3:
            tmp_list = line.split("|")
            log.setRevision(tmp_list[0])
            log.setAuthor(tmp_list[1])
            log.setTime(tmp_list[2])
        else:
            log.setMessage(line)
#    print log
    return log

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

def decode_passwd(cipher):
    text = base64.b64decode(cipher)
#    print cipher, text
    clear = text[2:5]+text[7:len(text)]
#    print clear
    return clear

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
#        print name, url, user, passwd
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

def main():
    xml_file = "../bin/config.xml"
    repo_list = svn_from_xml(xml_file)

    mysql_info = mysql_conninfo(xml_file)
    if mysql_info is None:
        print "not mysql connect info."
        sys.exit(1)
    conn = MySQLdb.connect(mysql_info[0], mysql_info[1], mysql_info[2], mysql_info[3], charset='utf8mb4')

#   记录执行的SQL语句
    f = open("./svnstat.sql", "a+");

    for repo in repo_list:
        repo_name =repo[0]
        url = repo[1]
        user = repo[2]
        passwd = repo[3]
        print("begin to query repo:%s, url:%s" % (repo_name, url))
        maxrev = repo[4]+1
        startrev = repo[5]
        for rev in range(startrev, maxrev):
            print("qeury revision %d log..." % (rev))
            log = svn_log_split(url, rev, user, passwd)
            if log.getRevision() == 0:
                print "svn rev #%d has not log" % (log.getRevision())
                continue
            try:
                cursor = conn.cursor()
                sql = "call pSvnStatNote('%s', %d, '%s', '%s', '%s')" % (repo_name, log.getRevision(), log.getTime(), log.getAuthor(), log.getMessage())
                result = cursor.execute(sql)
#                print sql, '[result='+str(result)+']'
                f.write("sql [%s], result [%d]\n" % (sql, result))
                conn.commit()
                cursor.close()
            except MySQLdb.Error, e:
                print "MySQL Error, err:%d, msg:%s" %(e.args[0], e.args[1])
                sys.exit(1)
    conn.close()
    f.close()

if __name__ == '__main__':
    main()


