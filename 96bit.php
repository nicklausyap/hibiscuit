<?php
// This source file is part of the hibiscuit open source project
// Date created: Monday, 1 April, 2024
// Copyright (c) 2024 nicklaus yap ken yik (nicklausyap)
// This is the php implementation of hibiscuit for 96-bit hash
// version 1.1

define('CHUNK_SIZE', 512);
define('NUM_WORDS', 16);

function rightRotate($value, $shift) {
    return (($value >> $shift) | ($value << (32 - $shift))) & 0xFFFFFFFF;
}

function doHash($words, &$h0, &$h1, &$h2, &$h3, &$h4, &$h5) {
    $a = $h0;
    $b = $h1;
    $c = $h2;
    $d = $h3;
    $e = $h4;
    $f = $h5;
    $temp = 0;
    $p = 0;
    $k = 0;
    for ($i = 0; $i < 60; $i++) {
        if ($i >= 0 && $i <= 19) {
            $p = ($b & $c) | (~$b & $d);
            $k = 0x5A82799A;
        } elseif ($i >= 20 && $i <= 39) {
            $p = $b ^ $c ^ $d;
            $k = 0x80000000;
        } elseif ($i >= 40 && $i <= 59) {
            $p = ($b & $c) | ($b & $d) | ($c & $d);
            $k = 0x9CC470A0;
        }
        $temp = rightRotate($a, 5) + $p + $e + $k + $words[$i % NUM_WORDS];
        $e = $d;
        $d = $c;
        $c = rightRotate($b, 30);
        $b = $f;
        $a = $temp & 0xFFFFFFFF;
    }
    $h0 = ($h0 + $a) & 0xFFFFFFFF;
    $h1 = ($h1 + $b) & 0xFFFFFFFF;
    $h2 = ($h2 + $c) & 0xFFFFFFFF;
    $h3 = ($h3 + $d) & 0xFFFFFFFF;
    $h4 = ($h4 + $e) & 0xFFFFFFFF;
    $h5 = ($h5 + $f) & 0xFFFFFFFF;
}

function breakIntoWords($chunk) {
    $words = [];
    for ($i = 0; $i < NUM_WORDS; $i++) {
        $words[$i] = (ord($chunk[$i * 4]) << 24) |
                     (ord($chunk[$i * 4 + 1]) << 16) |
                     (ord($chunk[$i * 4 + 2]) << 8) |
                     ord($chunk[$i * 4 + 3]);
        $words[$i] = $words[$i] & 0xFFFFFFFF;
    }
    return $words;
}

function breakInto512BitChunks($inputString) {
    $h0 = 0x67452301;
    $h1 = 0xEFCDAB89;
    $h2 = 0x98BADCFE;
    $h3 = 0xC3D2A1F0;
    $h4 = 0x10325476;
    $h5 = 0x1645931C;
    $inputLength = strlen($inputString);
    $totalLength = $inputLength * 8;
    $paddingBits = (CHUNK_SIZE - (($totalLength + 64) % CHUNK_SIZE)) % CHUNK_SIZE;
    $paddingBytes = ($paddingBits + 64) / 8;
    $paddedLength = $inputLength + $paddingBytes;
    $paddedString = $inputString . chr(0x80) . str_repeat(chr(0), $paddingBytes - 9);
    $bitLength = $inputLength * 8;
    $bitLengthPacked = pack('N*', 0, $bitLength); // Assuming a 64-bit length, adjust if necessary
    $paddedString .= str_pad($bitLengthPacked, 8, chr(0), STR_PAD_LEFT);

    for ($i = 0; $i < strlen($paddedString) / (CHUNK_SIZE / 8); $i++) {
        $chunk = substr($paddedString, $i * (CHUNK_SIZE / 8), CHUNK_SIZE / 8);
        $words = breakIntoWords($chunk);
       
        doHash($words, $h0, $h1, $h2, $h3, $h4, $h5);
    }
    // Print the final hash in hexadecimal format
    printf("Final hash: %08x%08x%08x\n", $h0, $h4, $h5);
}

// Below is an example input
$inputString = "He knew it was going to be a bad day when he saw mountain lions roaming the streets.Italy is my favorite country; in fact, I plan to spend two weeks there next year.";
breakInto512BitChunks($inputString);
