<?php
	if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_FILES['file'])) {
		$file = $_FILES['file'];
	
		// File information
		$fileName = $file['name'];
		$fileSize = $file['size'];
		$fileTmpPath = $file['tmp_name'];
		$fileError = $file['error'];
	
		// Check for errors
		if ($fileError === UPLOAD_ERR_OK) {
			// Specify the destination directory to save the uploaded file
			$uploadDir = $_SERVER["UPLOAD_TMP_DIR"] . "/Users/afaris/Desktop/web/upload/";
	
			// Generate a unique file name to avoid collisions
			$uniqueFileName = uniqid() . '_' . $fileName;
	
			// Move the uploaded file to the destination directory
			$destination = $uploadDir . $uniqueFileName;
			if (move_uploaded_file($fileTmpPath, $destination)) {
				echo "File uploaded successfully.";
			} else {
				echo "Error moving uploaded file.";
			}
		} else {
			echo "Error uploading file. Error code: " . $fileError;
		}
	}
	?>
	
	<!DOCTYPE html>
	<html>
	<head>
		<title>File Upload</title>
	</head>
	<body>
		<h2>Upload a File</h2>
		<form method="POST" enctype="multipart/form-data">
			<input type="file" name="file" required>
			<button type="submit">Upload</button>
		</form>
	</body>
	</html>