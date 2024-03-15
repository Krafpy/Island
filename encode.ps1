param (
    [switch]$Clean,
    [string]$OutName = "capture"
)

ffmpeg `
    -f s16le -ar 44100 -ac 2 -i "./capture/audio.raw" `
    -f image2 -framerate 60 -i "./capture/frame_%06d.png" `
    "$OutName.mp4"

if($Clean) {
    rmdir .\capture
}