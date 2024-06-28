const express = require('express');
const app = express();
const PORT = 3000;

// Sample JSON data (replace with your actual data handling)
let jsonData = {};

// Endpoint to handle GET requests for specific attributes
app.get('/:attribute', (req, res) => {
    const attribute = req.params.attribute.toLowerCase(); // Convert to lowercase
    if (jsonData.hasOwnProperty(attribute)) {
        res.json({ [attribute]: jsonData[attribute] });
    } else {
        res.status(404).json({ error: `${attribute} not found` });
    }
});

// POST endpoint to receive data from the C++ program
app.post('/receive_data', express.json(), (req, res) => {
    jsonData = req.body;
    console.log('Received updated data:', jsonData);
    res.status(200).send('Data received successfully');
});

// Start the server
app.listen(PORT, () => {
    console.log(`Server is running on http://localhost:${PORT}`);
});
