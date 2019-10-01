基本概念:
    HEAD指针:
        指向你正在工作中的本地分支的指针
参考资料:
    https://www.liaoxuefeng.com/wiki/0013739516305929606dd18361248578c67b8067c8c017b000/001374831943254ee90db11b13d4ba9a73b9047f4fb968d000
    //关于如何打补丁的
    https://www.devroom.io/2009/10/26/how-to-create-and-apply-a-patch-with-git/
    //关于zip包不含.git问题解决
    https://blog.csdn.net/m0_38028800/article/details/82015077
撤销修改:
    1.只修改了工作区(Working Directory)但未提交暂存区
    2.已提交到暂存区(unstage)
        //取消暂存区的修改
        git reset HEAD <file>
        //回退工作区的修改
        git checkout -- readme.txt
    3.已提交到本地仓库()
        git reflog
        git reset --hard commitId

git config --global color.status auto 
git config --global color.diff auto 
git config --global color.branch auto 
git config --global color.interactive auto 

分支创建: 
    git branch testing
分支切换:
    git checkout <分支名>
分支合并:
    合并分支:
        git merge dev
    删除分支:
        git branch -d dev
当前工作区暂存(stash):
    git stash
    git stash list
    恢复缓存:
        git stash apply stash@{0}
    删除缓存:
        git stash drop
    恢复并删除:
        git stash pop
    查看指定stash
        git stash show stash@{0}
        在该命令后面添加-p或--patch可以查看特定stash的全部diff，如下
    暂存时添加tag:
         git stash save "this is func3"
将文件从git仓库中删除,但保留本地文件,即删除其在跟踪清单中的记录
    git rm --cached <file name>
    git rm log/\*.log
    更新.gitignore
打标签:
    对当前最新的commit:
        git tag v1.0
    对历史版本添加:
        git reflog
        git tag v0.9 f52c633
    查看tag:
        git show <tagname>
    删除标签:
        git tag -d v0.1
    推送tag至远端服务器:
        git push origin <tagname>
    一次性推送全部尚未推送到远程的本地标签:
        git push origin --tags
    删除远端tag:
        git tag -d v0.9
        git push origin :refs/tags/v0.9
使用已经配置好的.gitignore
    https://github.com/github/gitignore
设置别名:
    $ git config --global alias.st status
    $ git config --global alias.co checkout
    $ git config --global alias.ci commit
    $ git config --global alias.br branch
    $ git config --global alias.unstage 'reset HEAD'
    $ git config --global alias.last 'log -1'
合并两个commit:
    git rebase -i HEAD~2
    对该分支的root进行重定向
    git rebase -i --root
去除git对文件模式的跟踪:
    git config core.filemode false
    注意:
        增加以上配置后,需将对应的head指针,重新指向最新一次提交才可,命令如下
            git reset --hard <最新一次提交的commit id>
差异对比:
    git diff 比较的是工作区和暂存区的差别
    git diff --cached 比较的是暂存区和版本库的差别
    git diff HEAD 可以查看工作区和版本库的差别
添加远程仓库地址:
    git remote add origin https://github.com/solo-king/lt8912b.git
patch about:
    //gen patch 
    git format-patch master --stdout > xxx.patch
    //根据第一个commit生成补丁
    git format-patch -1
    //根据第从第一个commit开始生成N个补丁
    git format-patch -N
    //check patch wether conflict, 使用前要检查喜爱
    git apply --check 1.patch
    //apply patch
    git am -s 1.patch
    //对于添加补丁有冲突的情况
        1.先强制添加补丁
        git am -s --reject 1.patch
        2.根据生成的*.rej文件手动修改冲突
        3. 修改完成后使用 git add .
        4. 最后 git am --continue 即可
git log:
--author=<pattern>, --committer=<pattern>
--grep=<pattern>


            

    
