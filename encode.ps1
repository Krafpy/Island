param (
    [switch]$Clean,
    [string]$OutName = "capture",
    [switch]$AudioOnly,
    [switch]$VideoOnly
)

if($AudioOnly -and $VideoOnly) {
    Write-Host "Invalid combination of options"
    return
}

if($AudioOnly) {
    ffmpeg `
        -f s16le -ar 44100 -ac 2 -i "./capture/audio.raw" `
        "$OutName.mp3"
}
elseif($VideoOnly) {
    ffmpeg `
        -f image2 -framerate 60 -i "./capture/frame_%06d.png" `
        "$OutName.mp4"
}
else {
    ffmpeg `
        -f s16le -ar 44100 -ac 2 -i "./capture/audio.raw" `
        -f image2 -framerate 60 -i "./capture/frame_%06d.png" `
        "$OutName.mp4"
}

if($Clean) {
    rmdir .\capture
}