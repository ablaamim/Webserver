<?php
// // Start a session to store session variables
// session_start();

// // Check if the form has been submitted
// if ($_SERVER['REQUEST_METHOD'] === 'POST') {
//     // Get the submitted username and password
//     $username = $_POST['username'];
//     $password = $_POST['password'];
    
//     // Check if the username and password are correct
//     if ($username === 'myusername' && $password === 'mypassword') {
//         // Set a cookie with the username
//         setcookie('username', $username, time() + 3600); // Expires in 1 hour

//         // Store the username in a session variable
//         $_SESSION['username'] = $username;

//         // Redirect the user to the home page
//         header('Location: home.php');
//         exit;
//     } else {
//         // Display an error message
//         echo 'Invalid username or password';
//     }
// }
?>
<?php
// Start a session to store session variables
session_start();

// Check if the form has been submitted
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Get the submitted username and password
    $username = $_POST['username'];
    $password = $_POST['password'];

    // Check if the username and password are correct
    if ($username === 'test' && $password === 'test') {
        // Set a cookie with the username
        setcookie('username', $username, time() + 3600); // Expires in 1 hour

        // Store the username in a session variable
        $_SESSION['username'] = $username;

        // Redirect the user to the home page
        header('Location: home.php');
        exit;
    } else {
        // Display an error message
        echo 'Invalid username or password';
    }
}
?>