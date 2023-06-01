<?php
session_start();

if ($_SERVER['REQUEST_METHOD'] == 'POST') {

    $avatar = $_FILES['avatar'];
    $avatar_name = $avatar['name'];
    $avatar_tmp_name = $avatar['tmp_name'];
    $avatar_size = $avatar['size'];
    $avatar_error = $avatar['error'];

    $avatar_ext = explode('.', $avatar_name);
    $avatar_actual_ext = strtolower(end($avatar_ext));

    $allowed = array('jpg', 'jpeg', 'png');

    if (in_array($avatar_actual_ext, $allowed)) {
        if ($avatar_error === 0) {
            if ($avatar_size < 1000000) {
                $avatar_name_new = uniqid('', true) . '.' . $avatar_actual_ext;
                $avatar_destination = 'uploads/' . $avatar_name_new;
                move_uploaded_file($avatar_tmp_name, $avatar_destination);
                $_SESSION['name'] = $_POST['name'];
                $_SESSION['email'] = $_POST['email'];
                $_SESSION['avatar'] = $avatar_destination;
            } else {
                echo 'File too big';
                exit(1);
            }
        } else {
            echo 'Error';
            exit(1);
        }
    } else {
        echo 'Not allowed';
        exit(1);
    }
} else if ($_SERVER['REQUEST_METHOD'] == 'GET' && isset($_GET['logout'])) {
    unset($_SESSION['name']);
    unset($_SESSION['email']);
    unset($_SESSION['avatar']);
    session_destroy();
}
?>

<!DOCTYPE html>
<div>
    <?php if (isset($_SESSION['name']) && isset($_SESSION['email'])): ?>
        <p>
            <center><h1>Hello, <?= $_SESSION['name'] ?>!</h1></center><hr>
            <center><img src="<?= $_SESSION['avatar'] ?>" alt="avatar" width="300" height="300" style="object-fit: cover;"></center><hr><br/>
            <center><?php echo $_SESSION['name']; ?></center>
            <center><?php echo $_SESSION['email']; ?></center><br/><br/>
            <center><a href="?logout">Logout</a></center>
        </p>
    <?php else: ?>
        <form method="post" enctype="multipart/form-data">
            <input type="text" name="name" placeholder="Name" />
            <input type="text" name="email" placeholder="Email" />
            <input type="file" id="avatar" name="avatar" accept="image/png, image/jpeg" />
            <input type="submit" value="Submit" />
        </form>
    <?php endif; ?>
</div>