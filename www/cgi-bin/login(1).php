<?php
session_start();

// Check if the user is already logged in
if (isset($_SESSION['username'])) {
    // Redirect to the login or any other page
    // header("Location: login.php");
    // exit();
}

// Check if the login form is submitted
if (isset($_POST['login'])) {
    // Simulate username and password validation
    $username = 'admin';
    $password = 'password';

    $inputUsername = $_POST['username'];
    $inputPassword = $_POST['password'];

    if ($inputUsername === $username && $inputPassword === $password) {
        // Set the session variables
        $_SESSION['username'] = $username;

        // Redirect to the login or any other page
        // header("Location: login.php");
        // exit();
    } else {
        // Invalid credentials
        $error = "Invalid username or password";
    }
}

// Check if the logout button is clicked
if (isset($_POST['logout'])) {
    // Clear all session data
    session_unset();

    // Destroy the session
    session_destroy();
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Login</title>
</head>
<body>
    <?php if (isset($_SESSION['username'])) { ?>
        <h2>Welcome, <?php echo $_SESSION['username']; ?></h2>
        <form method="POST" action="">
            <input type="submit" name="logout" value="Logout">
        </form>
    <?php } else { ?>
        <h2>Login</h2>
        <?php if (isset($error)) { ?>
            <p><?php echo $error; ?></p>
        <?php } ?>
        <form method="POST" action="">
            <label for="username">Username:</label>
            <input type="text" name="username" id="username" required><br>

            <label for="password">Password:</label>
            <input type="password" name="password" id="password" required><br>

            <input type="submit" name="login" value="Login">
        </form>
    <?php } ?>
</body>
</html>