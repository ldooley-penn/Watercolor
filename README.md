# Project Planning

I will be implementing the following paper:
https://maverick.inria.fr/Publications/2006/BKTS06/watercolor.pdf

## Inspiration and Goals
I am mainly inspired by many art galleries I have visited while travelling that depict nature scenes in watercolor and oil paintings. To achieve this I wish to simulate watercolor rendering firstly on static images and then expand to dynamic scenes if time allows.

## Implementation Steps
1. Set up application for image processing
2. Image abstraction
2a. Mean-shift color segmentation: https://comaniciu.net/Papers/MsAnalysis.pdf
2b. General abstraction via. Morphological smoothing filter (open + close). W/ user-editable kernel size (optional)
3. Implement wobbling
4. (Optional) Implement dry brush
5. Implement Edge darkening
6. Implement pigment density variation
7. Once verified, port to unreal as a plugin
8. Implement toon shading
9. Implement normal smoothing (edit-time operation)

## Timeline

### Week 1 (11/14)
Goal: Project setup and implement mean shift color segmentation algorithm

### Week 2 (11/21)
Goal: Wobbling and Edge Darkening

### Week 3 (11/28)
Goal: Pigment density variation + Port to Unreal or implement Morphological Smoothing

### Final Submission

Goal: Port to Unreal with toon shading and normal smoothing

# Milestone 1

Please use commit f9ee863ab8ab3e568236e3408dd9fc902e877250 which was made on November 10th as my milestone submission. At this stage I found the following results:

## Progress
So far things have been going very well, and I forsee the mean shift implementation to have been one of the biggest roadblocks since it was conceptually challening for me to understand. I forsee wobbling and the watercolor effects to be more simple to implement, but if I choose to implement morphological smoothing, that will be hard to do with a full RGB image.

## Mean shift segmentation in LUV color space
![mountains](https://github.com/user-attachments/assets/892e11ad-d7ee-454a-a297-d45d2fabaadd)
<img width="1919" height="1008" alt="AbstractedMountain" src="https://github.com/user-attachments/assets/2f9c9180-f8be-4c86-97dd-494c629bcc76" />


