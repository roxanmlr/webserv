#!/usr/bin/env php
<?php
/**
 * PHP CGI Test Script
 * Equivalent du script Python CGI
 */

// Fonction pour lire le corps de la requête
function read_body() {
    $content_length = 0;
    if (isset($_SERVER['CONTENT_LENGTH'])) {
        $content_length = intval($_SERVER['CONTENT_LENGTH']);
    }
    
    if ($content_length > 0) {
        return file_get_contents('php://input');
    }
    
    return "";
}

// Récupération des variables d'environnement/serveur
$method = isset($_SERVER['REQUEST_METHOD']) ? $_SERVER['REQUEST_METHOD'] : "";
$query_string = isset($_SERVER['QUERY_STRING']) ? $_SERVER['QUERY_STRING'] : "";
$content_type = isset($_SERVER['CONTENT_TYPE']) ? $_SERVER['CONTENT_TYPE'] : "";
$content_length = isset($_SERVER['CONTENT_LENGTH']) ? $_SERVER['CONTENT_LENGTH'] : "";

// Lecture du corps de la requête
$body = read_body();

// Parsing des paramètres de la query string
$query_params = [];
if (!empty($query_string)) {
    parse_str($query_string, $query_params);
}

// En-tête HTTP
header("Content-Type: text/html");
?>

<!DOCTYPE html>
<html>
<head>
    <title>PHP CGI Test</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        h1 { color: #333; }
        h2 { color: #666; margin-top: 20px; }
        ul { list-style-type: none; padding-left: 0; }
        li { margin: 5px 0; }
        pre { 
            background-color: #f5f5f5; 
            padding: 10px; 
            border: 1px solid #ddd; 
            border-radius: 4px;
            overflow-x: auto;
        }
    </style>
</head>
<body>

<h1>CGI PHP OK</h1>

<h2>Request info</h2>
<ul>
    <li>Method: <?php echo htmlspecialchars($method); ?></li>
    <li>Query string: <?php echo htmlspecialchars($query_string); ?></li>
    <li>Content-Type: <?php echo htmlspecialchars($content_type); ?></li>
    <li>Content-Length: <?php echo htmlspecialchars($content_length); ?></li>
</ul>

<h2>Query parameters</h2>
<pre>
<?php print_r($query_params); ?>
</pre>

<h2>Body</h2>
<pre>
<?php echo htmlspecialchars($body); ?>
</pre>

<h2>Environment</h2>
<pre>
<?php
// Affichage de toutes les variables d'environnement triées
$env_vars = $_SERVER;
ksort($env_vars);
foreach ($env_vars as $key => $value) {
    echo htmlspecialchars("$key=$value") . "\n";
}
?>
</pre>

</body>
</html>
