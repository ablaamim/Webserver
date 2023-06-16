<?php 
echo "Set-Cookie: text/html\r\n";
echo "Content-type: text/html\r\n\r\n"
?>
<!-- <form action="http://127.0.0.1:9090/www/cgi-bin/index.py" method="post">
 <p>Your f: <input type="text" name="l"></p>
 <p>Your l: <input type="text" name="f"></p>
 <p><input type="submit" value= "test"></p>
</form> -->
<form action="http://127.0.0.1:9090/www/cgi-bin/test-php.php" method="post">
 <p>Your id: <input type="text" name="id"></p>
 <p><input type="submit" value= "test"></p>
</form>