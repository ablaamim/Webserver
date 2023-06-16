<?php
echo "Set-Cookie: text/html\r\n";
echo "Content-type: text/html\r\n\r\n";

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    echo"get is here<br>";
    if(isset($_POST['id']))
        echo "your id is " + $_POST['id'] + "<br>";
    else
        echo "pas de id<br>";
}
if ($_SERVER['REQUEST_METHOD'] === 'GET') {
    echo"get is here<br>";
    if($_GET['id'])
        echo "your id is " + $_GET['id'] + "<br>";
    else
        echo "pas de idsdfzdfdzfds<br>";
}
?>