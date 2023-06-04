import cgi
import cgitb
import http.cookies
import os
import urllib.parse

# Get the query string from the environment variable
query_string = os.environ['QUERY_STRING']

# Parse the query string into an array of variables
query_vars = dict(urllib.parse.parse_qsl(query_string))

# Set the appropriate headers for the response
print("Content-Type: text/html")
print()
# Check if there are query string variables
if query_vars:
    # Convert the query variables to HTML
    html = "<h1>Query String Variables:</h1>"
    for key, value in query_vars.items():
        html += f"<p>{key} = {value}</p>"
else:
    html = "<center><h1>Sadge :(</h1><h1>No query string variables found.</h1></center>"

# Send the HTML as the response
print(html)
