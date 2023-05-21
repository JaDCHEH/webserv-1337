<?php
    // Get the query string from the environment variable
    header('QUERY_STRING' . $_SERVER['QUERY_STRING']);
    $queryString = $_SERVER['QUERY_STRING'];

    // Parse the query string into an array of variables
    parse_str($queryString, $queryVars);

    // Set the appropriate headers for the response
    header('Content-Type: text/html');
    
    // Set the FILENAME and SCRIPT_FILENAME headers
    header('SCRIPT_NAME: ' . $_SERVER['SCRIPT_NAME']);
    header('SCRIPT_FILENAME: ' . $_SERVER['SCRIPT_FILENAME']);

    // Convert the query variables to HTML
    $html = "<h1>Query String Variables:</h1>";
    foreach ($queryVars as $key => $value) {
        $html .= "<p>$key = $value</p>";
    }

    // Calculate the Content-Length header
    $contentLength = strlen($html);
    header('Content-Length: ' . $contentLength);

    // Send the HTML as the response
    echo $html;
?>