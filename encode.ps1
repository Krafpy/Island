param (
    [switch]$Clean
)

ffmpeg -f image2 -framerate 60 -i "./capture/frame_%06d.png" capture.mp4

if($Clean) {
    rmdir .\capture
}