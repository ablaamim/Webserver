#!/bin/bash

# Define an array of email addresses
declare -a EMAILS=(
    "afaris@e2r3p7.1337.ma"
    "afaris@e2r3p11.1337.ma"
    "afaris@e2r5p12.1337.ma"
    "afaris@e1r7p4.1337.ma"
    "afaris@e1r12p12.1337.ma"
    "afaris@e2r10p2.1337.ma"
    "afaris@e1r12p3.1337.ma"
    "afaris@e2r6p3.1337.ma"
    "afaris@e2r8p11.1337.ma"
    "afaris@e2r8p3.1337.ma"
    "afaris@e2r10p9.1337.ma"
    "afaris@e2r12p10.1337.ma"
    "afaris@e2r8p4.1337.ma"
    "afaris@e2r8p8.1337.ma"
    "afaris@e1r10p8.1337.ma"
    "afaris@e2r9p1.1337.ma"
    "afaris@e2r10p12.1337.ma"
    "afaris@e2r13p1.1337.ma"
    "afaris@e2r4p12.1337.ma"
  # Add more email addresses here...
)

NEW_NAME="Achraf Faris"
NEW_EMAIL="afaris@student.1337.ma"

# Loop through the email addresses
for WRONG_EMAIL in "${EMAILS[@]}"
do
  echo "Updating commits for email: $WRONG_EMAIL"

  # Execute the git filter-branch command
  git filter-branch --env-filter "
  if [ \"\$GIT_COMMITTER_EMAIL\" = \"$WRONG_EMAIL\" ]
  then
      export GIT_COMMITTER_NAME=\"$NEW_NAME\"
      export GIT_COMMITTER_EMAIL=\"$NEW_EMAIL\"
  fi
  if [ \"\$GIT_AUTHOR_EMAIL\" = \"$WRONG_EMAIL\" ]
  then
      export GIT_AUTHOR_NAME=\"$NEW_NAME\"
      export GIT_AUTHOR_EMAIL=\"$NEW_EMAIL\"
  fi
  " --tag-name-filter cat -- --branches --tags
done
