git filter-branch -f  --commit-filter '
        if [ "$GIT_COMMITTER_NAME" = "Achraf Faris" ];
        then
                GIT_COMMITTER_NAME="Achraf Faris";
                GIT_AUTHOR_NAME="Achraf Faris";
                GIT_COMMITTER_EMAIL="afaris@student.1337.ma";
                GIT_AUTHOR_EMAIL="afaris@student.1337.ma";
                git commit-tree "$@";
        else
                git commit-tree "$@";
        fi' HEAD