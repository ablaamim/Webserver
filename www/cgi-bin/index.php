<?php
  // Get the value of the 'name' parameter from the query string
  //$name = $_SERVER;

  // get from the query params name
  //print_r($_SERVER);
  if ($_SERVER['REQUEST_METHOD'] == 'POST')
  {
    var_dump($_POST);
    if(isset($_POST['id']))
      echo $_POST['id'];
    else
      echo "ps de poste<br>";
  }
  if ($_SERVER['REQUEST_METHOD'] == 'GET')
  {
    if(isset($_GET['id']))
      echo $_GET['id'];
    else
      echo "ps de get<br>";
  }
  //name = $_GET['name'];
  //while(1);
  echo "hello world<br>";
?>