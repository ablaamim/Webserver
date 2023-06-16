#!/usr/bin/python

# Import modules for CGI handling 
import cgi, cgitb 

# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
f = form.getvalue('f')
l  = form.getvalue('l')

print ("Set-Cookie: text/html\r\n")
print ("Content-type: text/html; charset=utf-8\r\n\r\n")
print ("<html>")
print ("<head>")
print ("<title>Hello - Second CGI Program</title>")
print ("</head>")
print ("<body>")
print ("<h2>Hello %s %s</h2>"%(f, l))
print ("</body>")
print ("</html>")