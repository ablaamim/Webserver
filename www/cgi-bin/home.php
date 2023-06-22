<?php
// Start the session to retrieve session variables
session_start();
?>

<!DOCTYPE html>
<html>
<head>
    <title>Home Page</title>
</head>
<body>
    <?php if (isset($_COOKIE['username']) && isset($_SESSION['username'])): ?>
        <p>Welcome, <?php echo $_COOKIE['username']; ?>!</p>
    <?php else: ?>
        <p>You are not logged in.</p>
    <?php endif; ?>
</body>
</html>