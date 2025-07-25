# ==========================================================================
#
#   Copyright NumFOCUS
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#          https://www.apache.org/licenses/LICENSE-2.0.txt
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
# ==========================================================================


import itk
from sys import argv

itk.auto_progress(2)

image = itk.imread(argv[1])

reoriented_image = itk.orient_image_filter(
    image,
    use_image_direction=True,
    desired_coordinate_orientation=itk.AnatomicalOrientation(
        itk.AnatomicalOrientation.PositiveEnum_RAS
    ),
)

itk.imwrite(reoriented_image, argv[2])
