<html>
<head>
<meta charset="UTF-8" content="text/html">
<title>練習用フォーム</title>
</head>

<body>
<h1>練習用フォーム(2.確認画面)</h1>
<p>内容を確認してください</p>

<?php
//入力値の取得
$uname = $_POST["uname"];
$email = $_POST["email"];
$message = $_POST["message"];
?>

<form method="POST" action="submit.php">
  <table border="1">
    <tr>
      <td>お名前</td>
      <td><?php echo $uname; ?></td>
    </tr>
    <tr>
       <td>メールアドレス</td>
       <td><?php echo $email; ?></td>
    </tr>
    <tr>
      <td>メッセージ</td>
      <td><?php echo nl2br($message); ?></td>
    </tr>
    <tr>
      <td colspan="2"><input type="submit" name="sub1" value="送信する"></td>
    </tr>
</table>
<!--hiddenフィールド-->
<input type="hidden" name="uname" value="<?php echo $uname; ?>">
<input type="hidden" name="email" value="<?php echo $email; ?>">
<input type="hidden" name="message" value="<?php echo $message; ?>">
</form>
</body>
</html>
