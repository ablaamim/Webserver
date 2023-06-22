<?php
  if ($_SERVER['REQUEST_METHOD'] == 'POST')
  {
    if(isset($_POST['id']))
      echo $_POST['id']."<br>";
    else
      echo "pas de post id<br>";
  }
  if ($_SERVER['REQUEST_METHOD'] == 'GET')
  {
    if(isset($_GET['id']))
      echo $_GET['id']."<br>";
    else
      echo "pas de get id <br>";
  }
?>