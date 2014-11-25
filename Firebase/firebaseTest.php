<?php
require_once 'firebaseLib.php';
// --- This is your Firebase URL
$url = 'https://radiant-torch-2376.firebaseio.com/';
// --- Use your token from Firebase here
$token = 'xxxxxxxxxxxxxxx';
// --- Here is your parameter from the http GET
$arduino_data = $_GET['arduino_data'];
// --- $arduino_data_post = $_POST['name'];
// --- Set up your Firebase url structure here
$firebasePath = '/rpmnodestatus/';
/// --- Making calls
$fb = new fireBase($url);
$response = $fb->set($firebasePath, $arduino_data);
sleep(2);