<?php  
if (isset($_POST["submit"])):
    $target_path = $_SERVER["UPLOAD_TMP_DIR"];  
    echo $_SERVER["UPLOAD_TMP_DIR"] . "\n";
    $target_path = $target_path."/".basename( $_FILES['fileToUpload']['name']);   
    
    if(move_uploaded_file($_FILES['fileToUpload']['tmp_name'], $target_path)) {  
        echo "File uploaded successfully!";  
    } else{  
        echo "Sorry, file not uploaded, please try again!";  
    }  
endif;
?>  
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
<form method="post" enctype="multipart/form-data">  
    Select File:  
    <input type="file" name="fileToUpload"/>  
    <input type="submit" value="Upload Image" name="submit"/>  
</form>  
</body>
</html>