<?php
  // Get the value of the 'name' parameter from the query string
  $name = $_SERVER;

  // get from the query params name
  $name = $_POST['name'];
  // print it 
  echo $name;
  // Output a message with the value of the 'name' parameter
   print_r($_SERVER);
?>