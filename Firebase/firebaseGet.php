<?php
require_once 'firebaseLib.php';
// --- This is your Firebase URL
$url = 'https://radiant-torch-2376.firebaseio.com/';
// --- Here is your parameter from the http GET
$deviceId = $_GET['deviceId'];
// --- Set up your Firebase url structure here
$firebasePath = '/rpmnodestatus/node1/';
/// --- Making calls
$fb = new fireBase($url);
$response = $fb->get($firebasePath);
sleep(2);