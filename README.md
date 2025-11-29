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

# Milestone 2

Please use commit 6db821aaf1459f42d3664ac574c19f5cd3b62e71 which was made on November 24th as my milestone submission. At this stage I had the following results:

## Progress
Now I have the full image shading pipeline complete as presented in the original research paper and am in the process of working on adding in toon shaded models.

## Full image pipeline results
<img width="1919" height="1006" alt="WatercolorShadingCheckpoint2" src="https://github.com/user-attachments/assets/5846b6e7-29d2-464d-b159-f9e363ebba30" />

# Final Submission

## Results

Mountains raw image:
![mountains_nice_sky](https://github.com/user-attachments/assets/f686bd9d-5f10-4911-afd8-a0fa57eef43b)

Mountains after processing:
<img width="1280" height="858" alt="watercolor_mountains_nice_sky" src="https://github.com/user-attachments/assets/5ad44900-9a9f-4abe-a698-de5f599f68cd" />

Mesh after processing:
<img width="635" height="475" alt="watercolor_bunny" src="https://github.com/user-attachments/assets/80d992e9-c8c3-4ae9-b8a6-08d4f1355e86" />

## Post Mortem

Overall this project went fairly well. Some of the more challenging pieces such as 
the LUV color space conversion and Mean Shift Segmentation were the first things
I tackled which got them out of the way and de-risked the project. 

I did follow the paper's suggestions on how they implemented various layers of noise functions
to mimic certain watercolor effects, but I'm not 100% in love with how they look. I think
the layered gaussian noise looks somewhat nice for high frequency noise, but I wouldn't really
relate it to watercolor painting as much as just a way to try abstracting the image so the
viewer sees it as being less realistic. However, I would say overall learning about a handful
of extra noise types was a good learning experience and I can see how they will come in handy
in future projects.

During the later parts of the project, I wanted to test out the paper's realtime mesh
pipeline as well and did so just through my OpenGL application directly. I think the output
is OK, but not as good as the image -> watercolor pipeline. I think the main reasons for this are
that toon shading, which replaces the mean shift segementation from the image pipeline, does not
create the same effect since it can leave some pretty low frequency regions that are unnatural for
watercolor. I'm sure it would also look better with textured models, but I did not end up implementing
those. 

Overall, I could definitely see this being an interesting effect in something like a game application
if the pre-watercolor effect pipeline is designed well to segment into medium to high frequency
segments. I also think that figuring out a way to blur the edge darkening effect in one direction
would make the "color pooling" effect of the stylization more effective.
