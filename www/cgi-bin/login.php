<?php
    session_start();
    if(isset($_SESSION['username']) && !empty($_SESSION['username'])) {
        header('Location: home.php');
    }
?>
<!DOCTYPE html>
<html>
<head>
    <title>Login Form</title>
</head>
<body>
    <form action="authenticate.php" method="post">
        <label for="username">Username:</label>
        <input type="text" id="username" name="username"><br>

        <label for="password">Password:</label>
        <input type="password" id="password" name="password"><br>

        <input type="submit" value="Login">
    </form>
    <?php
        if (isset($_COOKIE['error'])) {
            echo '<p class="error" style="color:red">' . $_COOKIE['error'] . '</p>';
        }
    ?>
</body>
</html>