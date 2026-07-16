<script setup lang="ts">
import { computed, onBeforeUnmount, onMounted, ref, shallowRef } from 'vue'
import { init, use, type EChartsType } from 'echarts/core'
import { CanvasRenderer } from 'echarts/renderers'
import { GraphChart } from 'echarts/charts'
import { LegendComponent, TimelineComponent, TitleComponent, TooltipComponent } from 'echarts/components'
import type { GraphSeriesOption } from 'echarts/charts'
import type { EChartsOption } from 'echarts'

use([CanvasRenderer, GraphChart, LegendComponent, TimelineComponent, TitleComponent, TooltipComponent])

type NodeCategory = '课程' | '领域' | '构念' | '方法' | '理论' | '应用场景'

type CourseRecord = {
  id: string
  name: string
  category: string
  stage: string
  order: number
  prerequisites: string[]
  coveredCount: number
  coveredIds: string[]
  coveredNames: string[]
  description: string
  time: '大一' | '大二' | '大三' | '大三或大四' | '大四'
}

type GraphNode = {
  id: string
  name: string
  category: NodeCategory
  symbolSize: number
  value?: string
  x?: number
  y?: number
  fixed?: boolean
}

type GraphLink = {
  source: string
  target: string
  name: string
}

const relationColors: Record<string, string> = {
  覆盖: '#0ea5e9',
  前置于: '#f97316',
  从属于: '#64748b',
  应用于: '#22c55e',
  影响: '#e11d48',
  测量: '#8b5cf6',
  演变: '#d97706',
  解释: '#14b8a6',
}

const extraRelations: GraphLink[] = [
  { source: 'D002', target: 'D001', name: '从属于' },
  { source: 'D003', target: 'D001', name: '从属于' },
  { source: 'D004', target: 'D001', name: '从属于' },
  { source: 'D005', target: 'D001', name: '从属于' },
  { source: 'D006', target: 'D001', name: '从属于' },
  { source: 'D007', target: 'D001', name: '从属于' },
  { source: 'D008', target: 'D001', name: '从属于' },
  { source: 'D009', target: 'D001', name: '从属于' },
  { source: 'D010', target: 'D003', name: '从属于' },
  { source: 'D011', target: 'D003', name: '从属于' },
  { source: 'D012', target: 'D003', name: '从属于' },
  { source: 'D013', target: 'D003', name: '从属于' },
  { source: 'D014', target: 'D003', name: '从属于' },
  { source: 'D015', target: 'D005', name: '从属于' },
  { source: 'D016', target: 'D004', name: '从属于' },
  { source: 'D017', target: 'D008', name: '从属于' },
  { source: 'D018', target: 'D007', name: '从属于' },
  { source: 'H001', target: 'H004', name: '演变' },
  { source: 'H002', target: 'H005', name: '演变' },
  { source: 'H004', target: 'H005', name: '演变' },
  { source: 'H006', target: 'H009', name: '演变' },
  { source: 'H007', target: 'T001', name: '演变' },
  { source: 'H007', target: 'T002', name: '演变' },
  { source: 'H008', target: 'T003', name: '演变' },
  { source: 'H010', target: 'T004', name: '演变' },
  { source: 'H010', target: 'T006', name: '演变' },
  { source: 'H010', target: 'T008', name: '演变' },
  { source: 'H011', target: 'T029', name: '演变' },
  { source: 'H012', target: 'T028', name: '演变' },
  { source: 'H003', target: 'K002', name: '演变' },
  { source: 'H009', target: 'T019', name: '演变' },
  { source: 'T001', target: 'D012', name: '从属于' },
  { source: 'T002', target: 'D012', name: '从属于' },
  { source: 'T003', target: 'D010', name: '从属于' },
  { source: 'T004', target: 'D003', name: '从属于' },
  { source: 'T005', target: 'D012', name: '从属于' },
  { source: 'T006', target: 'D012', name: '从属于' },
  { source: 'T007', target: 'D013', name: '从属于' },
  { source: 'T008', target: 'D013', name: '从属于' },
  { source: 'T009', target: 'D013', name: '从属于' },
  { source: 'T010', target: 'D016', name: '从属于' },
  { source: 'T011', target: 'D016', name: '从属于' },
  { source: 'T012', target: 'D004', name: '从属于' },
  { source: 'T013', target: 'D004', name: '从属于' },
  { source: 'T014', target: 'D005', name: '从属于' },
  { source: 'T015', target: 'D015', name: '从属于' },
  { source: 'T016', target: 'D015', name: '从属于' },
  { source: 'T017', target: 'D015', name: '从属于' },
  { source: 'T018', target: 'D006', name: '从属于' },
  { source: 'T019', target: 'D014', name: '从属于' },
  { source: 'T020', target: 'D018', name: '从属于' },
  { source: 'T021', target: 'D018', name: '从属于' },
  { source: 'T022', target: 'D018', name: '从属于' },
  { source: 'T023', target: 'D018', name: '从属于' },
  { source: 'T024', target: 'D017', name: '从属于' },
  { source: 'T025', target: 'D017', name: '从属于' },
  { source: 'T026', target: 'D008', name: '从属于' },
  { source: 'T027', target: 'D014', name: '从属于' },
  { source: 'T028', target: 'D014', name: '从属于' },
  { source: 'T029', target: 'D008', name: '从属于' },
  { source: 'T030', target: 'D009', name: '从属于' },
  { source: 'T031', target: 'D009', name: '从属于' },
  { source: 'T032', target: 'D018', name: '从属于' },
  { source: 'C001', target: 'D002', name: '从属于' },
  { source: 'C002', target: 'D002', name: '从属于' },
  { source: 'C003', target: 'D002', name: '从属于' },
  { source: 'C004', target: 'D002', name: '从属于' },
  { source: 'C005', target: 'D002', name: '从属于' },
  { source: 'C006', target: 'D002', name: '从属于' },
  { source: 'C007', target: 'D002', name: '从属于' },
  { source: 'C008', target: 'D002', name: '从属于' },
  { source: 'C009', target: 'D002', name: '从属于' },
  { source: 'C010', target: 'D010', name: '从属于' },
  { source: 'C011', target: 'D010', name: '从属于' },
  { source: 'C012', target: 'D010', name: '从属于' },
  { source: 'C013', target: 'D010', name: '从属于' },
  { source: 'C014', target: 'D010', name: '从属于' },
  { source: 'C015', target: 'D010', name: '从属于' },
  { source: 'C016', target: 'D010', name: '从属于' },
  { source: 'C017', target: 'D010', name: '从属于' },
  { source: 'C018', target: 'D011', name: '从属于' },
  { source: 'C019', target: 'D011', name: '从属于' },
  { source: 'C020', target: 'D011', name: '从属于' },
  { source: 'C021', target: 'D011', name: '从属于' },
  { source: 'C022', target: 'D011', name: '从属于' },
  { source: 'C023', target: 'D012', name: '从属于' },
  { source: 'C024', target: 'D012', name: '从属于' },
  { source: 'C025', target: 'D012', name: '从属于' },
  { source: 'C026', target: 'D012', name: '从属于' },
  { source: 'C027', target: 'D012', name: '从属于' },
  { source: 'C028', target: 'D012', name: '从属于' },
  { source: 'C029', target: 'D012', name: '从属于' },
  { source: 'C030', target: 'D012', name: '从属于' },
  { source: 'C031', target: 'D012', name: '从属于' },
  { source: 'C032', target: 'D012', name: '从属于' },
  { source: 'C033', target: 'D012', name: '从属于' },
  { source: 'C034', target: 'D012', name: '从属于' },
  { source: 'C035', target: 'D013', name: '从属于' },
  { source: 'C036', target: 'D013', name: '从属于' },
  { source: 'C037', target: 'D013', name: '从属于' },
  { source: 'C038', target: 'D013', name: '从属于' },
  { source: 'C039', target: 'D013', name: '从属于' },
  { source: 'C040', target: 'D013', name: '从属于' },
  { source: 'C041', target: 'D013', name: '从属于' },
  { source: 'C042', target: 'D013', name: '从属于' },
  { source: 'C043', target: 'D013', name: '从属于' },
  { source: 'C044', target: 'D013', name: '从属于' },
  { source: 'C045', target: 'D013', name: '从属于' },
  { source: 'C046', target: 'D013', name: '从属于' },
  { source: 'C047', target: 'D013', name: '从属于' },
  { source: 'C048', target: 'D013', name: '从属于' },
  { source: 'C049', target: 'D014', name: '从属于' },
  { source: 'C050', target: 'D014', name: '从属于' },
  { source: 'C051', target: 'D014', name: '从属于' },
  { source: 'C052', target: 'D014', name: '从属于' },
  { source: 'C053', target: 'D014', name: '从属于' },
  { source: 'C054', target: 'D014', name: '从属于' },
  { source: 'C055', target: 'D014', name: '从属于' },
  { source: 'C056', target: 'D014', name: '从属于' },
  { source: 'C057', target: 'D014', name: '从属于' },
  { source: 'C058', target: 'D014', name: '从属于' },
  { source: 'C059', target: 'D014', name: '从属于' },
  { source: 'C060', target: 'D014', name: '从属于' },
  { source: 'C061', target: 'D004', name: '从属于' },
  { source: 'C062', target: 'D004', name: '从属于' },
  { source: 'C063', target: 'D004', name: '从属于' },
  { source: 'C064', target: 'D016', name: '从属于' },
  { source: 'C065', target: 'D004', name: '从属于' },
  { source: 'C066', target: 'D004', name: '从属于' },
  { source: 'C067', target: 'D004', name: '从属于' },
  { source: 'C068', target: 'D004', name: '从属于' },
  { source: 'C069', target: 'D004', name: '从属于' },
  { source: 'C070', target: 'D004', name: '从属于' },
  { source: 'C071', target: 'D016', name: '从属于' },
  { source: 'C072', target: 'D004', name: '从属于' },
  { source: 'C073', target: 'D016', name: '从属于' },
  { source: 'C074', target: 'D016', name: '从属于' },
  { source: 'C075', target: 'D016', name: '从属于' },
  { source: 'C076', target: 'D005', name: '从属于' },
  { source: 'C077', target: 'D005', name: '从属于' },
  { source: 'C078', target: 'D005', name: '从属于' },
  { source: 'C079', target: 'D005', name: '从属于' },
  { source: 'C080', target: 'D005', name: '从属于' },
  { source: 'C081', target: 'D005', name: '从属于' },
  { source: 'C082', target: 'D015', name: '从属于' },
  { source: 'C083', target: 'D015', name: '从属于' },
  { source: 'C084', target: 'D015', name: '从属于' },
  { source: 'C085', target: 'D015', name: '从属于' },
  { source: 'C086', target: 'D006', name: '从属于' },
  { source: 'C087', target: 'D006', name: '从属于' },
  { source: 'C088', target: 'D006', name: '从属于' },
  { source: 'C089', target: 'D006', name: '从属于' },
  { source: 'C090', target: 'D006', name: '从属于' },
  { source: 'C091', target: 'D006', name: '从属于' },
  { source: 'C092', target: 'D006', name: '从属于' },
  { source: 'C093', target: 'D006', name: '从属于' },
  { source: 'C094', target: 'D015', name: '从属于' },
  { source: 'C095', target: 'D015', name: '从属于' },
  { source: 'C096', target: 'D015', name: '从属于' },
  { source: 'C097', target: 'D015', name: '从属于' },
  { source: 'C098', target: 'D015', name: '从属于' },
  { source: 'C099', target: 'D015', name: '从属于' },
  { source: 'C100', target: 'D015', name: '从属于' },
  { source: 'C101', target: 'D007', name: '从属于' },
  { source: 'C102', target: 'D007', name: '从属于' },
  { source: 'C103', target: 'D007', name: '从属于' },
  { source: 'C104', target: 'D007', name: '从属于' },
  { source: 'C105', target: 'D007', name: '从属于' },
  { source: 'C106', target: 'D007', name: '从属于' },
  { source: 'C107', target: 'D007', name: '从属于' },
  { source: 'C108', target: 'D007', name: '从属于' },
  { source: 'C109', target: 'D007', name: '从属于' },
  { source: 'C110', target: 'D007', name: '从属于' },
  { source: 'C111', target: 'D007', name: '从属于' },
  { source: 'C112', target: 'D007', name: '从属于' },
  { source: 'C113', target: 'D007', name: '从属于' },
  { source: 'C114', target: 'D007', name: '从属于' },
  { source: 'C115', target: 'D007', name: '从属于' },
  { source: 'C116', target: 'D008', name: '从属于' },
  { source: 'C117', target: 'D008', name: '从属于' },
  { source: 'C118', target: 'D008', name: '从属于' },
  { source: 'C119', target: 'D008', name: '从属于' },
  { source: 'C120', target: 'D008', name: '从属于' },
  { source: 'C121', target: 'D008', name: '从属于' },
  { source: 'C122', target: 'D008', name: '从属于' },
  { source: 'C123', target: 'D008', name: '从属于' },
  { source: 'C124', target: 'D008', name: '从属于' },
  { source: 'C125', target: 'D008', name: '从属于' },
  { source: 'C126', target: 'D008', name: '从属于' },
  { source: 'C127', target: 'D008', name: '从属于' },
  { source: 'C128', target: 'D008', name: '从属于' },
  { source: 'C129', target: 'D008', name: '从属于' },
  { source: 'C130', target: 'D008', name: '从属于' },
  { source: 'C131', target: 'D009', name: '从属于' },
  { source: 'C132', target: 'D009', name: '从属于' },
  { source: 'C133', target: 'D009', name: '从属于' },
  { source: 'C134', target: 'D009', name: '从属于' },
  { source: 'C135', target: 'D009', name: '从属于' },
  { source: 'C136', target: 'D009', name: '从属于' },
  { source: 'C137', target: 'D009', name: '从属于' },
  { source: 'C138', target: 'D009', name: '从属于' },
  { source: 'C139', target: 'D009', name: '从属于' },
  { source: 'C140', target: 'D009', name: '从属于' },
  { source: 'M001', target: 'D009', name: '从属于' },
  { source: 'M002', target: 'D009', name: '从属于' },
  { source: 'M003', target: 'D009', name: '从属于' },
  { source: 'M004', target: 'D009', name: '从属于' },
  { source: 'M005', target: 'D009', name: '从属于' },
  { source: 'M006', target: 'D009', name: '从属于' },
  { source: 'M007', target: 'D009', name: '从属于' },
  { source: 'M008', target: 'D009', name: '从属于' },
  { source: 'M009', target: 'D009', name: '从属于' },
  { source: 'M010', target: 'D009', name: '从属于' },
  { source: 'M011', target: 'D009', name: '从属于' },
  { source: 'M012', target: 'D009', name: '从属于' },
  { source: 'M013', target: 'D009', name: '从属于' },
  { source: 'M014', target: 'D009', name: '从属于' },
  { source: 'M015', target: 'D009', name: '从属于' },
  { source: 'M016', target: 'D009', name: '从属于' },
  { source: 'M017', target: 'D009', name: '从属于' },
  { source: 'M018', target: 'D009', name: '从属于' },
  { source: 'M019', target: 'D009', name: '从属于' },
  { source: 'M020', target: 'D009', name: '从属于' },
  { source: 'M021', target: 'D009', name: '从属于' },
  { source: 'M022', target: 'D009', name: '从属于' },
  { source: 'M023', target: 'D009', name: '从属于' },
  { source: 'M024', target: 'D009', name: '从属于' },
  { source: 'M025', target: 'D009', name: '从属于' },
  { source: 'M026', target: 'D009', name: '从属于' },
  { source: 'M027', target: 'D009', name: '从属于' },
  { source: 'M028', target: 'D009', name: '从属于' },
  { source: 'M029', target: 'D009', name: '从属于' },
  { source: 'M030', target: 'D009', name: '从属于' },
  { source: 'M031', target: 'D009', name: '从属于' },
  { source: 'M032', target: 'D009', name: '从属于' },
  { source: 'M033', target: 'D009', name: '从属于' },
  { source: 'M034', target: 'D009', name: '从属于' },
  { source: 'M035', target: 'D009', name: '从属于' },
  { source: 'M036', target: 'D009', name: '从属于' },
  { source: 'M037', target: 'D009', name: '从属于' },
  { source: 'M038', target: 'D009', name: '从属于' },
  { source: 'M039', target: 'D009', name: '从属于' },
  { source: 'M040', target: 'D009', name: '从属于' },
  { source: 'M041', target: 'D009', name: '从属于' },
  { source: 'M042', target: 'D009', name: '从属于' },
  { source: 'M043', target: 'D009', name: '从属于' },
  { source: 'M044', target: 'D009', name: '从属于' },
  { source: 'M045', target: 'D009', name: '从属于' },
  { source: 'T001', target: 'C033', name: '解释' },
  { source: 'T001', target: 'C034', name: '解释' },
  { source: 'T002', target: 'C054', name: '解释' },
  { source: 'T002', target: 'C101', name: '解释' },
  { source: 'T002', target: 'C112', name: '解释' },
  { source: 'T003', target: 'C017', name: '解释' },
  { source: 'T003', target: 'C013', name: '解释' },
  { source: 'T003', target: 'C015', name: '解释' },
  { source: 'T004', target: 'C031', name: '解释' },
  { source: 'T004', target: 'C032', name: '解释' },
  { source: 'T004', target: 'C035', name: '解释' },
  { source: 'T004', target: 'C043', name: '解释' },
  { source: 'T005', target: 'C023', name: '解释' },
  { source: 'T005', target: 'C025', name: '解释' },
  { source: 'T005', target: 'C026', name: '解释' },
  { source: 'T005', target: 'C027', name: '解释' },
  { source: 'T006', target: 'C024', name: '解释' },
  { source: 'T006', target: 'C035', name: '解释' },
  { source: 'T006', target: 'C048', name: '解释' },
  { source: 'T007', target: 'C011', name: '解释' },
  { source: 'T007', target: 'C012', name: '解释' },
  { source: 'T007', target: 'C040', name: '解释' },
  { source: 'T008', target: 'C041', name: '解释' },
  { source: 'T008', target: 'C042', name: '解释' },
  { source: 'T008', target: 'C036', name: '解释' },
  { source: 'T009', target: 'C041', name: '解释' },
  { source: 'T009', target: 'C042', name: '解释' },
  { source: 'T009', target: 'C103', name: '解释' },
  { source: 'T010', target: 'C062', name: '解释' },
  { source: 'T010', target: 'C063', name: '解释' },
  { source: 'T011', target: 'C064', name: '解释' },
  { source: 'T011', target: 'C071', name: '解释' },
  { source: 'T011', target: 'C074', name: '解释' },
  { source: 'T012', target: 'C061', name: '解释' },
  { source: 'T012', target: 'C067', name: '解释' },
  { source: 'T013', target: 'C069', name: '解释' },
  { source: 'T013', target: 'C072', name: '解释' },
  { source: 'T014', target: 'C058', name: '解释' },
  { source: 'T014', target: 'C098', name: '解释' },
  { source: 'T014', target: 'C099', name: '解释' },
  { source: 'T015', target: 'C078', name: '解释' },
  { source: 'T015', target: 'C079', name: '解释' },
  { source: 'T016', target: 'C076', name: '解释' },
  { source: 'T016', target: 'C054', name: '解释' },
  { source: 'T016', target: 'C126', name: '解释' },
  { source: 'T017', target: 'C082', name: '解释' },
  { source: 'T017', target: 'C084', name: '解释' },
  { source: 'T017', target: 'C095', name: '解释' },
  { source: 'T018', target: 'C088', name: '解释' },
  { source: 'T018', target: 'C089', name: '解释' },
  { source: 'T018', target: 'C090', name: '解释' },
  { source: 'T018', target: 'C091', name: '解释' },
  { source: 'T018', target: 'C092', name: '解释' },
  { source: 'T018', target: 'C093', name: '解释' },
  { source: 'T019', target: 'C055', name: '解释' },
  { source: 'T019', target: 'C056', name: '解释' },
  { source: 'T019', target: 'C058', name: '解释' },
  { source: 'T019', target: 'C101', name: '解释' },
  { source: 'T020', target: 'C059', name: '解释' },
  { source: 'T020', target: 'C101', name: '解释' },
  { source: 'T020', target: 'C108', name: '解释' },
  { source: 'T021', target: 'C103', name: '解释' },
  { source: 'T021', target: 'C108', name: '解释' },
  { source: 'T021', target: 'C112', name: '解释' },
  { source: 'T022', target: 'C113', name: '解释' },
  { source: 'T022', target: 'C114', name: '解释' },
  { source: 'T022', target: 'C121', name: '解释' },
  { source: 'T023', target: 'C105', name: '解释' },
  { source: 'T023', target: 'C106', name: '解释' },
  { source: 'T023', target: 'C108', name: '解释' },
  { source: 'T024', target: 'C117', name: '解释' },
  { source: 'T024', target: 'C118', name: '解释' },
  { source: 'T024', target: 'C129', name: '解释' },
  { source: 'T025', target: 'C052', name: '解释' },
  { source: 'T025', target: 'C117', name: '解释' },
  { source: 'T025', target: 'C118', name: '解释' },
  { source: 'T025', target: 'C124', name: '解释' },
  { source: 'T025', target: 'C125', name: '解释' },
  { source: 'T026', target: 'C119', name: '解释' },
  { source: 'T026', target: 'C127', name: '解释' },
  { source: 'T027', target: 'C050', name: '解释' },
  { source: 'T027', target: 'C108', name: '解释' },
  { source: 'T028', target: 'C051', name: '解释' },
  { source: 'T028', target: 'C052', name: '解释' },
  { source: 'T028', target: 'C053', name: '解释' },
  { source: 'T029', target: 'C126', name: '解释' },
  { source: 'T029', target: 'C122', name: '解释' },
  { source: 'T029', target: 'C119', name: '解释' },
  { source: 'T030', target: 'C137', name: '解释' },
  { source: 'T030', target: 'C140', name: '解释' },
  { source: 'T031', target: 'C133', name: '解释' },
  { source: 'T031', target: 'C134', name: '解释' },
  { source: 'T031', target: 'C139', name: '解释' },
  { source: 'T032', target: 'C111', name: '解释' },
  { source: 'T032', target: 'C109', name: '解释' },
  { source: 'T032', target: 'C110', name: '解释' },
  { source: 'C051', target: 'C041', name: '影响' },
  { source: 'C024', target: 'C041', name: '影响' },
  { source: 'C024', target: 'C047', name: '影响' },
  { source: 'C058', target: 'C101', name: '影响' },
  { source: 'C058', target: 'C112', name: '影响' },
  { source: 'C094', target: 'C121', name: '影响' },
  { source: 'C113', target: 'C103', name: '影响' },
  { source: 'C113', target: 'C114', name: '影响' },
  { source: 'C107', target: 'C102', name: '影响' },
  { source: 'C105', target: 'C106', name: '影响' },
  { source: 'C095', target: 'C098', name: '影响' },
  { source: 'C084', target: 'C100', name: '影响' },
  { source: 'C119', target: 'C127', name: '影响' },
  { source: 'C121', target: 'C126', name: '影响' },
  { source: 'C024', target: 'C044', name: '影响' },
  { source: 'C071', target: 'C075', name: '影响' },
  { source: 'C073', target: 'C074', name: '影响' },
  { source: 'C076', target: 'C126', name: '影响' },
  { source: 'C089', target: 'C100', name: '影响' },
  { source: 'C091', target: 'C108', name: '影响' },
  { source: 'C092', target: 'C117', name: '影响' },
  { source: 'C093', target: 'C047', name: '影响' },
  { source: 'C051', target: 'C121', name: '影响' },
  { source: 'M025', target: 'C076', name: '测量' },
  { source: 'M025', target: 'C087', name: '测量' },
  { source: 'M025', target: 'C103', name: '测量' },
  { source: 'M025', target: 'C122', name: '测量' },
  { source: 'M025', target: 'C133', name: '测量' },
  { source: 'M025', target: 'C134', name: '测量' },
  { source: 'M026', target: 'C116', name: '测量' },
  { source: 'M026', target: 'C123', name: '测量' },
  { source: 'M026', target: 'C125', name: '测量' },
  { source: 'M027', target: 'C017', name: '测量' },
  { source: 'M027', target: 'C018', name: '测量' },
  { source: 'M027', target: 'C024', name: '测量' },
  { source: 'M027', target: 'C035', name: '测量' },
  { source: 'M027', target: 'C041', name: '测量' },
  { source: 'M028', target: 'C018', name: '测量' },
  { source: 'M028', target: 'C024', name: '测量' },
  { source: 'M028', target: 'C035', name: '测量' },
  { source: 'M029', target: 'C018', name: '测量' },
  { source: 'M029', target: 'C017', name: '测量' },
  { source: 'M029', target: 'C010', name: '测量' },
  { source: 'M030', target: 'C021', name: '测量' },
  { source: 'M030', target: 'C049', name: '测量' },
  { source: 'M030', target: 'C051', name: '测量' },
  { source: 'M031', target: 'C005', name: '测量' },
  { source: 'M031', target: 'C024', name: '测量' },
  { source: 'M031', target: 'C041', name: '测量' },
  { source: 'M032', target: 'C024', name: '测量' },
  { source: 'M032', target: 'C051', name: '测量' },
  { source: 'M032', target: 'C071', name: '测量' },
  { source: 'M033', target: 'C050', name: '测量' },
  { source: 'M033', target: 'C119', name: '测量' },
  { source: 'M033', target: 'C117', name: '测量' },
  { source: 'M034', target: 'C018', name: '测量' },
  { source: 'M034', target: 'C036', name: '测量' },
  { source: 'M035', target: 'C036', name: '测量' },
  { source: 'M035', target: 'C035', name: '测量' },
  { source: 'M036', target: 'C018', name: '测量' },
  { source: 'M036', target: 'C021', name: '测量' },
  { source: 'M037', target: 'C085', name: '测量' },
  { source: 'M037', target: 'C083', name: '测量' },
  { source: 'M038', target: 'C122', name: '测量' },
  { source: 'M038', target: 'C103', name: '测量' },
  { source: 'M038', target: 'C115', name: '测量' },
  { source: 'M039', target: 'C049', name: '测量' },
  { source: 'M039', target: 'C117', name: '测量' },
  { source: 'M039', target: 'C118', name: '测量' },
  { source: 'M040', target: 'C123', name: '测量' },
  { source: 'M040', target: 'C110', name: '测量' },
  { source: 'M040', target: 'C126', name: '测量' },
  { source: 'M041', target: 'C088', name: '测量' },
  { source: 'M041', target: 'C123', name: '测量' },
  { source: 'M042', target: 'C088', name: '测量' },
  { source: 'M042', target: 'C111', name: '测量' },
  { source: 'M043', target: 'C095', name: '测量' },
  { source: 'M043', target: 'C106', name: '测量' },
  { source: 'M043', target: 'C015', name: '测量' },
  { source: 'M044', target: 'C101', name: '测量' },
  { source: 'M044', target: 'C103', name: '测量' },
  { source: 'M044', target: 'C126', name: '测量' },
  { source: 'M045', target: 'C024', name: '测量' },
  { source: 'M045', target: 'C041', name: '测量' },
  { source: 'M045', target: 'C047', name: '测量' },
  { source: 'C018', target: 'A001', name: '应用于' },
  { source: 'C024', target: 'A001', name: '应用于' },
  { source: 'C043', target: 'A001', name: '应用于' },
  { source: 'C074', target: 'A001', name: '应用于' },
  { source: 'C075', target: 'A001', name: '应用于' },
  { source: 'C051', target: 'A002', name: '应用于' },
  { source: 'C058', target: 'A002', name: '应用于' },
  { source: 'C073', target: 'A002', name: '应用于' },
  { source: 'C024', target: 'A003', name: '应用于' },
  { source: 'C033', target: 'A003', name: '应用于' },
  { source: 'C043', target: 'A003', name: '应用于' },
  { source: 'C131', target: 'A004', name: '应用于' },
  { source: 'C133', target: 'A004', name: '应用于' },
  { source: 'C134', target: 'A004', name: '应用于' },
  { source: 'C101', target: 'A005', name: '应用于' },
  { source: 'C105', target: 'A005', name: '应用于' },
  { source: 'C107', target: 'A005', name: '应用于' },
  { source: 'C108', target: 'A005', name: '应用于' },
  { source: 'C110', target: 'A006', name: '应用于' },
  { source: 'C111', target: 'A006', name: '应用于' },
  { source: 'C109', target: 'A006', name: '应用于' },
  { source: 'C103', target: 'A007', name: '应用于' },
  { source: 'C108', target: 'A007', name: '应用于' },
  { source: 'C112', target: 'A007', name: '应用于' },
  { source: 'C117', target: 'A008', name: '应用于' },
  { source: 'C118', target: 'A008', name: '应用于' },
  { source: 'C123', target: 'A008', name: '应用于' },
  { source: 'C125', target: 'A008', name: '应用于' },
  { source: 'C076', target: 'A009', name: '应用于' },
  { source: 'C094', target: 'A009', name: '应用于' },
  { source: 'C122', target: 'A009', name: '应用于' },
  { source: 'T016', target: 'A009', name: '应用于' },
  { source: 'T029', target: 'A009', name: '应用于' },
  { source: 'C017', target: 'A010', name: '应用于' },
  { source: 'C018', target: 'A010', name: '应用于' },
  { source: 'C048', target: 'A010', name: '应用于' },
  { source: 'C024', target: 'A011', name: '应用于' },
  { source: 'C035', target: 'A011', name: '应用于' },
  { source: 'C048', target: 'A011', name: '应用于' },
  { source: 'C130', target: 'A012', name: '应用于' },
  { source: 'C123', target: 'A012', name: '应用于' },
  { source: 'C119', target: 'A013', name: '应用于' },
  { source: 'C126', target: 'A013', name: '应用于' },
  { source: 'C127', target: 'A013', name: '应用于' },
  { source: 'T029', target: 'A013', name: '应用于' },
  { source: 'C073', target: 'A014', name: '应用于' },
  { source: 'C122', target: 'A014', name: '应用于' },
  { source: 'C121', target: 'A014', name: '应用于' },
  { source: 'C082', target: 'A015', name: '应用于' },
  { source: 'C084', target: 'A015', name: '应用于' },
  { source: 'C094', target: 'A015', name: '应用于' },
  { source: 'T017', target: 'A015', name: '应用于' },
  { source: 'M046', target: 'D009', name: '从属于' },
  { source: 'M046', target: 'C088', name: '测量' },
  { source: 'M046', target: 'C111', name: '测量' },
  { source: 'C141', target: 'D002', name: '从属于' },
  { source: 'C141', target: 'C024', name: '影响' },
  { source: 'C141', target: 'C018', name: '影响' },
  { source: 'T034', target: 'C141', name: '解释' },
  { source: 'C142', target: 'D002', name: '从属于' },
  { source: 'C142', target: 'C024', name: '影响' },
  { source: 'C142', target: 'C035', name: '影响' },
  { source: 'T034', target: 'C142', name: '解释' },
  { source: 'C143', target: 'D002', name: '从属于' },
  { source: 'C143', target: 'C074', name: '影响' },
  { source: 'C143', target: 'C121', name: '影响' },
  { source: 'C143', target: 'C126', name: '影响' },
  { source: 'C143', target: 'A001', name: '应用于' },
  { source: 'C143', target: 'A008', name: '应用于' },
  { source: 'C143', target: 'A013', name: '应用于' },
  { source: 'T034', target: 'C143', name: '解释' },
  { source: 'T033', target: 'D009', name: '从属于' },
  { source: 'T033', target: 'C133', name: '解释' },
  { source: 'T033', target: 'C134', name: '解释' },
  { source: 'T033', target: 'C139', name: '解释' },
  { source: 'T034', target: 'D003', name: '从属于' },
  { source: 'T034', target: 'C024', name: '解释' },
  { source: 'T034', target: 'C035', name: '解释' },
]


type KnowledgeSnapshot = {
  year: string
  courses: CourseRecord[]
  nodes: GraphNode[]
  links: GraphLink[]
  coverageCount: number
}

const courseNodeSize = 56

const courseData: CourseRecord[] = [
  {
    id: 'K001',
    name: '普通心理学',
    category: '基础必修',
    stage: '阶段1 导入与底层工具',
    order: 1,
    prerequisites: [],
    coveredCount: 14,
    coveredIds: ['D001', 'D002', 'D003', 'D004', 'D005', 'D006', 'D007', 'D008', 'C017', 'C024', 'C049', 'C076', 'C088', 'C116'],
    coveredNames: ['普通心理学', '生理心理学', '认知心理学', '发展心理学', '社会心理学', '人格心理学', '工业与组织心理学', '临床与健康心理学', '知觉', '工作记忆', '情绪', '态度', '人格特质', '心理障碍'],
    description: '心理学核心概念、领域与基本规律导入',
    time: '大一',
  },
  {
    id: 'K002',
    name: '心理学史与本土化视野',
    category: '基础必修',
    stage: '阶段1 导入与底层工具',
    order: 2,
    prerequisites: [],
    coveredCount: 12,
    coveredIds: ['H001', 'H002', 'H003', 'H004', 'H005', 'H006', 'H007', 'H008', 'H009', 'H010', 'H011', 'H012'],
    coveredNames: ['经验主义', '理性主义', '中国传统文化心理观', '构造主义', '机能主义', '精神分析', '行为主义', '格式塔', '认知革命', '人本主义', '进化心理学', '本土心理学'],
    description: '心理学流派演变及本土化议题',
    time: '大一',
  },
  {
    id: 'K003',
    name: '现代心理统计学',
    category: '基础必修',
    stage: '阶段1 导入与底层工具',
    order: 3,
    prerequisites: [],
    coveredCount: 11,
    coveredIds: ['D009', 'C131', 'C135', 'C136', 'C137', 'M013', 'M014', 'M015', 'M016', 'M017', 'M018'],
    coveredNames: ['心理测量与研究方法', '变量', '样本', '效应量', '统计检验力', '描述统计', 't检验', '方差分析', '相关分析', '回归分析', '经典测量理论'],
    description: '描述统计、推断统计与心理数据分析基础',
    time: '大一',
  },
  {
    id: 'K004',
    name: '程序化数据分析',
    category: '基础必修',
    stage: '阶段1 导入与底层工具',
    order: 4,
    prerequisites: [],
    coveredCount: 7,
    coveredIds: ['M038', 'M039', 'M040', 'M041', 'M042', 'M044', 'C140'],
    coveredNames: ['文本挖掘', '情绪分析', '机器学习分类', '聚类分析', '降维分析', '行为日志分析', '开放科学实践'],
    description: '以R/Python完成数据清洗、可视化与基础分析',
    time: '大一',
  },
  {
    id: 'K026',
    name: 'Python语言程序设计',
    category: '基础必修',
    stage: '阶段1 导入与底层工具',
    order: 26,
    prerequisites: [],
    coveredCount: 0,
    coveredIds: [],
    coveredNames: [],
    description: '讲解Python基础语法与编程逻辑，培养代码读写能力',
    time: '大一',
  },
  {
    id: 'K007',
    name: '生理心理学',
    category: '核心必修',
    stage: '阶段2 核心支柱领域',
    order: 7,
    prerequisites: ['普通心理学'],
    coveredCount: 13,
    coveredIds: ['D002', 'C001', 'C002', 'C003', 'C004', 'C005', 'C006', 'C007', 'C008', 'C009', 'M030', 'M031', 'M032'],
    coveredNames: ['生理心理学', '神经元', '动作电位', '突触传递', '神经递质', '脑区功能定位', '内分泌调节', '遗传率', '昼夜节律', '睡眠阶段', 'EEG/ERP', 'fMRI', 'fNIRS'],
    description: '神经系统、生理机制与行为关系',
    time: '大一',
  },
  {
    id: 'K008',
    name: '认知心理学',
    category: '核心必修',
    stage: '阶段2 核心支柱领域',
    order: 8,
    prerequisites: ['普通心理学'],
    coveredCount: 12,
    coveredIds: ['D003', 'D011', 'D012', 'D013', 'C018', 'C024', 'C025', 'C035', 'C041', 'C042', 'T006', 'T008'],
    coveredNames: ['认知心理学', '注意与意识', '记忆与学习', '语言与思维', '选择性注意', '工作记忆', '长时记忆', '执行功能', '决策', '认知偏差', '工作记忆模型', '双加工理论'],
    description: '注意、记忆、执行功能与决策',
    time: '大一',
  },
  {
    id: 'K009',
    name: '发展心理学',
    category: '核心必修',
    stage: '阶段2 核心支柱领域',
    order: 9,
    prerequisites: ['普通心理学'],
    coveredCount: 12,
    coveredIds: ['D004', 'D016', 'C061', 'C062', 'C063', 'C064', 'C069', 'C071', 'C072', 'T010', 'T011', 'T013'],
    coveredNames: ['发展心理学', '发展与教育', '依恋', '客体永久性', '守恒', '最近发展区', '认同发展', '语言习得', '认知老化', '皮亚杰认知发展理论', '维果茨基社会文化理论', '埃里克森心理社会发展理论'],
    description: '个体毕生发展及教育应用',
    time: '大二',
  },
  {
    id: 'K010',
    name: '社会心理学',
    category: '核心必修',
    stage: '阶段2 核心支柱领域',
    order: 10,
    prerequisites: ['普通心理学'],
    coveredCount: 12,
    coveredIds: ['D005', 'D015', 'C076', 'C078', 'C080', 'C081', 'C082', 'C083', 'C084', 'T015', 'T016', 'T017'],
    coveredNames: ['社会心理学', '社会认知与群体', '态度', '归因', '从众', '服从', '社会认同', '刻板印象', '偏见', '归因理论', '计划行为理论', '社会认同理论'],
    description: '态度、归因、群体过程与社会影响',
    time: '大二',
  },
  {
    id: 'K011',
    name: '人格心理学',
    category: '核心必修',
    stage: '阶段2 核心支柱领域',
    order: 11,
    prerequisites: ['普通心理学'],
    coveredCount: 10,
    coveredIds: ['D006', 'C086', 'C087', 'C088', 'C089', 'C090', 'C091', 'C092', 'C093', 'T018'],
    coveredNames: ['人格心理学', '自我概念', '自尊', '人格特质', '外向性', '宜人性', '尽责性', '神经质', '开放性', '大五人格模型'],
    description: '人格结构、特质与个体差异',
    time: '大二',
  },
  {
    id: 'K012',
    name: '临床与变态心理学',
    category: '核心必修',
    stage: '阶段2 核心支柱领域',
    order: 12,
    prerequisites: ['普通心理学'],
    coveredCount: 13,
    coveredIds: ['D008', 'D017', 'C116', 'C117', 'C118', 'C119', 'C120', 'C123', 'C124', 'C125', 'T024', 'T025', 'T029'],
    coveredNames: ['临床与健康心理学', '心理病理与干预', '心理障碍', '焦虑', '抑郁', '应激', '应对方式', '症状评估', '暴露', '治疗联盟', '应激-脆弱性模型', '认知行为治疗模型', '生物-心理-社会模型'],
    description: '心理病理、诊断思路与干预基础',
    time: '大二',
  },
  {
    id: 'K005',
    name: '实验心理学',
    category: '核心必修',
    stage: '阶段3 方法规范与知识生产',
    order: 14,
    prerequisites: ['现代心理统计学'],
    coveredCount: 8,
    coveredIds: ['M006', 'M007', 'M027', 'M028', 'M034', 'M035', 'M036', 'C138'],
    coveredNames: ['真实验', '准实验', '行为任务', '反应时记录', 'Stroop范式', 'Go/No-Go范式', '双耳分听范式', '因果推断'],
    description: '实验设计、变量控制与经典范式',
    time: '大二',
  },
  {
    id: 'K006',
    name: '心理测量学',
    category: '核心必修',
    stage: '阶段3 方法规范与知识生产',
    order: 15,
    prerequisites: ['现代心理统计学'],
    coveredCount: 7,
    coveredIds: ['C132', 'C133', 'C134', 'C139', 'M024', 'M025', 'M026'],
    coveredNames: ['操作性定义', '信度', '效度', '测量误差', '项目反应理论分析', '问卷量表编制', '结构化访谈'],
    description: '测量理论、信效度与量表开发',
    time: '大二',
  },
  {
    id: 'K027',
    name: '实验心理学实验',
    category: '核心必修',
    stage: '阶段3 方法规范与知识生产',
    order: 27,
    prerequisites: ['Python语言程序设计', '实验心理学'],
    coveredCount: 6,
    coveredIds: ['M006', 'M027', 'M028', 'M034', 'M035', 'M036'],
    coveredNames: ['真实验', '行为任务', '反应时记录', 'Stroop范式', 'Go/No-Go范式', '双耳分听范式'],
    description: '经典心理学实验编程',
    time: '大二',
  },
  {
    id: 'K013',
    name: '工业与组织心理学',
    category: '核心必修',
    stage: '阶段2 核心支柱领域',
    order: 13,
    prerequisites: ['普通心理学'],
    coveredCount: 20,
    coveredIds: ['D007', 'D018', 'C101', 'C102', 'C103', 'C105', 'C107', 'C108', 'C109', 'C110', 'C111', 'C112', 'C113', 'C114', 'C115', 'T020', 'T021', 'T022', 'T023', 'T032'],
    coveredNames: ['工业与组织心理学', '组织行为与人力资源', '工作动机', '组织承诺', '工作满意度', '领导力', '组织公平', '绩效', '招聘选拔', '胜任力', '人岗匹配', '培训迁移', '绩效反馈', '职业倦怠', '工作投入', '期望理论', '工作特征模型', '资源保存理论', '领导权变理论', '胜任力模型'],
    description: '工作动机、组织行为与人力资源',
    time: '大三',
  },
  {
    id: 'K014',
    name: '学术写作与科研伦理',
    category: '方法规范',
    stage: '阶段1 导入与底层工具',
    order: 5,
    prerequisites: [],
    coveredCount: 4,
    coveredIds: ['C132', 'C138', 'C140', 'T030'],
    coveredNames: ['操作性定义', '因果推断', '开放科学实践', '开放科学框架'],
    description: '科研写作、APA规范与伦理要求',
    time: '大三',
  },
  {
    id: 'K015',
    name: '开放科学实践',
    category: '方法规范',
    stage: '阶段1 导入与底层工具',
    order: 6,
    prerequisites: [],
    coveredCount: 4,
    coveredIds: ['C137', 'C140', 'T030', 'M010'],
    coveredNames: ['统计检验力', '开放科学实践', '开放科学框架', '元分析'],
    description: '预注册、数据共享与可重复性实践',
    time: '大三',
  },
  {
    id: 'K024',
    name: '大数据心理学研究方法',
    category: '前沿选修',
    stage: '阶段3 方法规范与知识生产',
    order: 16,
    prerequisites: ['程序化数据分析', '现代心理统计学'],
    coveredCount: 8,
    coveredIds: ['M038', 'M039', 'M040', 'M041', 'M042', 'M043', 'M044', 'M045'],
    coveredNames: ['文本挖掘', '情绪分析', '机器学习分类', '聚类分析', '降维分析', '社会网络分析', '行为日志分析', '认知计算建模'],
    description: '文本、日志与机器学习方法',
    time: '大三或大四',
  },
  {
    id: 'K017',
    name: '情绪心理学',
    category: '方向选修',
    stage: '阶段3 方法规范与知识生产',
    order: 17,
    prerequisites: ['认知心理学'],
    coveredCount: 8,
    coveredIds: ['D014', 'C049', 'C050', 'C051', 'C052', 'C053', 'T028', 'T027'],
    coveredNames: ['情绪与动机', '情绪', '情绪唤醒', '情绪调节', '认知重评', '表达抑制', '情绪调节过程模型', 'Yerkes-Dodson定律'],
    description: '情绪机制、测量与调节',
    time: '大三或大四',
  },
  {
    id: 'K018',
    name: '感觉与知觉',
    category: '方向选修',
    stage: '阶段3 方法规范与知识生产',
    order: 18,
    prerequisites: ['认知心理学'],
    coveredCount: 9,
    coveredIds: ['D010', 'C010', 'C011', 'C012', 'C013', 'C014', 'C015', 'T003', 'T007'],
    coveredNames: ['感觉与知觉', '感受器', '绝对阈限', '差别阈限', '知觉恒常性', '深度知觉', '错觉', '格式塔知觉组织原则', '信号检测理论'],
    description: '感觉系统与知觉组织过程',
    time: '大三或大四',
  },
  {
    id: 'K019',
    name: '心理语言学',
    category: '方向选修',
    stage: '阶段4 方向与应用拓展',
    order: 19,
    prerequisites: ['认知心理学'],
    coveredCount: 6,
    coveredIds: ['D013', 'C044', 'C045', 'C046', 'C071', 'T004'],
    coveredNames: ['语言与思维', '语言理解', '语言产生', '词汇提取', '语言习得', '信息加工理论'],
    description: '语言理解、产生与加工模型',
    time: '大三或大四',
  },
  {
    id: 'K022',
    name: '用户研究与人机交互',
    category: '应用选修',
    stage: '阶段4 方向与应用拓展',
    order: 20,
    prerequisites: ['认知心理学'],
    coveredCount: 8,
    coveredIds: ['A010', 'A011', 'C017', 'C018', 'C024', 'C048', 'M029', 'M044'],
    coveredNames: ['用户体验', '人机交互', '知觉', '选择性注意', '工作记忆', '认知负荷', '眼动仪', '行为日志分析'],
    description: '用户研究方法、认知负荷与界面设计',
    time: '大三或大四',
  },
  {
    id: 'K025',
    name: '人工智能与心理学专题',
    category: '前沿选修',
    stage: '阶段4 方向与应用拓展',
    order: 21,
    prerequisites: ['程序化数据分析', '认知心理学'],
    coveredCount: 6,
    coveredIds: ['A011', 'C035', 'C041', 'C048', 'M045', 'M040'],
    coveredNames: ['人机交互', '执行功能', '决策', '认知负荷', '认知计算建模', '机器学习分类'],
    description: 'AI与心理学交叉主题与案例',
    time: '大三或大四',
  },
  {
    id: 'K021',
    name: '教育心理学',
    category: '应用选修',
    stage: '阶段4 方向与应用拓展',
    order: 22,
    prerequisites: ['发展心理学'],
    coveredCount: 6,
    coveredIds: ['D016', 'C064', 'C073', 'C074', 'C075', 'T011'],
    coveredNames: ['发展与教育', '最近发展区', '社会情感学习', '学习迁移', '学业自我概念', '维果茨基社会文化理论'],
    description: '学习规律、教学设计与教育评价',
    time: '大三或大四',
  },
  {
    id: 'K020',
    name: '文化心理学',
    category: '方向选修',
    stage: '阶段4 方向与应用拓展',
    order: 23,
    prerequisites: ['社会心理学'],
    coveredCount: 6,
    coveredIds: ['H003', 'C065', 'C082', 'C086', 'K010', 'K011'],
    coveredNames: ['中国传统文化心理观', '社会化', '社会认同', '自我概念', '社会心理学', '人格心理学'],
    description: '文化情境中的心理过程与比较',
    time: '大三或大四',
  },
  {
    id: 'K023',
    name: '心理咨询实务',
    category: '应用选修',
    stage: '阶段4 方向与应用拓展',
    order: 24,
    prerequisites: ['临床与变态心理学'],
    coveredCount: 7,
    coveredIds: ['C117', 'C118', 'C124', 'C125', 'C130', 'M011', 'M026'],
    coveredNames: ['焦虑', '抑郁', '暴露', '治疗联盟', '数字疗法', '经验抽样法', '结构化访谈'],
    description: '基础咨询技能与个案工作',
    time: '大三或大四',
  },
  {
    id: 'K028',
    name: '磁共振成像原理和应用',
    category: '前沿选修',
    stage: '阶段4 方向与应用拓展',
    order: 28,
    prerequisites: ['生理心理学'],
    coveredCount: 5,
    coveredIds: ['M031', 'C005', 'C141', 'C142', 'C143'],
    coveredNames: ['fMRI', '脑区功能定位', '神经振荡', '脑连接', '大脑可塑性'],
    description: '核磁共振成像的原理、实验设计、数据处理和应用',
    time: '大三或大四',
  },
  {
    id: 'K029',
    name: '认知神经科学导论',
    category: '前沿选修',
    stage: '阶段4 方向与应用拓展',
    order: 29,
    prerequisites: ['生理心理学', '认知心理学'],
    coveredCount: 12,
    coveredIds: ['D002', 'D011', 'D012', 'C024', 'C035', 'M030', 'M031', 'M032', 'C005', 'C141', 'C142', 'C143'],
    coveredNames: ['生理心理学', '注意与意识', '记忆与学习', '工作记忆', '执行功能', 'EEG/ERP', 'fMRI', 'fNIRS', '脑区功能定位', '神经振荡', '脑连接', '大脑可塑性'],
    description: '认知神经系统的生理基础、脑网络与行为关系',
    time: '大三或大四',
  },
  {
    id: 'K030',
    name: '高级心理统计',
    category: '前沿选修',
    stage: '阶段3 方法规范与知识生产',
    order: 30,
    prerequisites: ['现代心理统计学'],
    coveredCount: 6,
    coveredIds: ['M018', 'M046', 'M021', 'M019', 'M020', 'T033'],
    coveredNames: ['经典测量理论', '回归分析', '因素分析', '中介分析', '调节分析', '结构方程模型'],
    description: '多元回归、因素分析与结构方程模型应用',
    time: '大三或大四',
  },
  {
    id: 'K016',
    name: '毕业论文',
    category: '整合实践',
    stage: '阶段4 方向与应用拓展',
    order: 25,
    prerequisites: ['现代心理统计学', '实验心理学', '心理测量学', '学术写作与科研伦理', '开放科学实践'],
    coveredCount: 6,
    coveredIds: ['K003', 'K004', 'K005', 'K006', 'K014', 'K015'],
    coveredNames: ['现代心理统计学', '程序化数据分析', '实验心理学', '心理测量学', '学术写作与科研伦理', '开放科学实践'],
    description: '完成实证研究或数据项目',
    time: '大四',
  },
]

const timelineSlots = [
  { year: '总览' },
  { year: '大一' },
  { year: '大二' },
  { year: '大三' },
  { year: '大四' },
] as const

const categories: { name: NodeCategory }[] = [
  { name: '课程' },
  { name: '领域' },
  { name: '构念' },
  { name: '方法' },
  { name: '理论' },
  { name: '应用场景' },
]

const legendItems: { name: NodeCategory; className: string }[] = [
  { name: '课程', className: 'course' },
  { name: '领域', className: 'domain' },
  { name: '构念', className: 'construct' },
  { name: '方法', className: 'method' },
  { name: '理论', className: 'theory' },
  { name: '应用场景', className: 'application' },
]

const relationLegendItems = Object.entries(relationColors).map(([name, color]) => ({ name, color }))

function inferCategory(id: string): NodeCategory {
  if (id.startsWith('K')) return '课程'
  if (id.startsWith('D')) return '领域'
  if (id.startsWith('M')) return '方法'
  if (id.startsWith('T') || id.startsWith('H')) return '理论'
  if (id.startsWith('A')) return '应用场景'
  return '构念'
}

const courseByName = new Map(courseData.map((course) => [course.name, course]))

function addNode(nodes: Map<string, GraphNode>, node: GraphNode) {
  const current = nodes.get(node.id)
  if (!current || current.symbolSize < node.symbolSize) {
    nodes.set(node.id, node)
  }
}

function getCoursesByYear(year: (typeof timelineSlots)[number]['year']) {
  if (year === '总览') {
    return [...courseData].sort((first, second) => first.order - second.order)
  }

  return courseData
    .filter((course) => course.time === year || (course.time === '大三或大四' && (year === '大三' || year === '大四')))
    .sort((first, second) => first.order - second.order)
}

function buildSnapshot(slot: (typeof timelineSlots)[number]): KnowledgeSnapshot {
  const courses = getCoursesByYear(slot.year)
  const nodes = new Map<string, GraphNode>()
  const links: GraphLink[] = []

  for (const course of courses) {
    addNode(nodes, {
      id: course.id,
      name: course.name,
      category: '课程',
      symbolSize: courseNodeSize,
      value: `${course.category}｜${course.stage}`,
    })

    for (const prerequisiteName of course.prerequisites) {
      const prerequisite = courseByName.get(prerequisiteName)
      if (!prerequisite) continue

      addNode(nodes, {
        id: prerequisite.id,
        name: prerequisite.name,
        category: '课程',
        symbolSize: courseNodeSize,
        value: `前置课程｜${prerequisite.category}`,
      })
      links.push({ source: prerequisite.id, target: course.id, name: '前置于' })
    }

    course.coveredIds.forEach((id, index) => {
      const category = inferCategory(id)
      const name = course.coveredNames[index] ?? id

      addNode(nodes, {
        id,
        name,
        category,
        symbolSize: category === '课程' ? courseNodeSize : 32,
        value: `由「${course.name}」覆盖`,
      })
      links.push({ source: course.id, target: id, name: '覆盖' })
    })
  }

  const visibleNodeIds = new Set(nodes.keys())
  const existingLinkIds = new Set(links.map((link) => `${link.source}->${link.target}->${link.name}`))

  for (const relation of extraRelations) {
    if (!visibleNodeIds.has(relation.source) || !visibleNodeIds.has(relation.target)) continue

    const linkId = `${relation.source}->${relation.target}->${relation.name}`
    if (existingLinkIds.has(linkId)) continue

    existingLinkIds.add(linkId)
    links.push(relation)
  }

  return {
    year: slot.year,
    courses,
    nodes: Array.from(nodes.values()),
    links,
    coverageCount: courses.reduce((sum, course) => sum + course.coveredCount, 0),
  }
}

function layoutNodes(snapshot: KnowledgeSnapshot): GraphNode[] {
  const groups: Record<NodeCategory, GraphNode[]> = {
    课程: [],
    领域: [],
    构念: [],
    方法: [],
    理论: [],
    应用场景: [],
  }

  for (const node of snapshot.nodes) {
    groups[node.category].push(node)
  }

  for (const group of Object.values(groups)) {
    group.sort((first, second) => first.name.localeCompare(second.name, 'zh-Hans-CN'))
  }

  const layoutGrid = (
    items: GraphNode[],
    startX: number,
    startY: number,
    columns: number,
    columnGap: number,
    rowGap: number,
  ) =>
    items.map((node, index) => ({
      ...node,
      x: startX + (index % columns) * columnGap,
      y: startY + Math.floor(index / columns) * rowGap,
      fixed: true,
    }))

  const isOverview = snapshot.year === '总览'

  return [
    ...layoutGrid(groups.领域, -1680, -540, 3, 330, 210),
    ...layoutGrid(groups.方法, -1680, isOverview ? 880 : 360, 3, 330, 210),
    ...layoutGrid(groups.课程, -580, -400, 3, 500, 330),
    ...layoutGrid(groups.构念, 750, -750, 6, 290, 210),
    ...layoutGrid(groups.理论, 890, isOverview ? 2740 : 1380, 4, 370, 230),
    ...layoutGrid(groups.应用场景, 890, isOverview ? 5320 : 2440, 4, 370, 230),
  ]
}

const snapshots = timelineSlots.map((slot) => buildSnapshot(slot)) as [KnowledgeSnapshot, ...KnowledgeSnapshot[]]

const activeIndex = ref(0)
const chartElement = ref<HTMLDivElement>()
const chart = shallowRef<EChartsType>()
let resizeObserver: ResizeObserver | undefined

const activeSnapshot = computed(() => snapshots[activeIndex.value] ?? snapshots[0])

type TooltipPayload = {
  dataType?: string
  data: {
    name?: string
    category?: string
    value?: string
    source?: string
    target?: string
  }
}

const makeGraphSeries = (snapshot: KnowledgeSnapshot): GraphSeriesOption => ({
  type: 'graph',
  layout: 'none',
  animation: false,
  roam: true,
  draggable: false,
  categories,
  data: layoutNodes(snapshot).map((node) => ({
    ...node,
    fixed: true,
    label: { show: true },
  })),
  links: snapshot.links.map((link) => ({
    ...link,
    lineStyle: {
      color: relationColors[link.name] ?? '#94a3b8',
      opacity: 0.72,
      width: link.name === '前置于' ? 2.2 : 1.5,
    },
  })),
  label: {
    color: '#17202a',
    fontSize: 11,
    fontWeight: 1000,
    position: 'bottom',
    distance: 4,
    width: 96,
    overflow: 'truncate',
  },
  edgeSymbol: ['none', 'arrow'],
  edgeSymbolSize: 7,
  edgeLabel: {
    show: false,
  },
  lineStyle: {
    color: '#94a3b8',
    curveness: 0.14,
    opacity: 0.62,
    width: 1.4,
  },
  emphasis: {
    focus: 'adjacency',
    label: {
      show: true,
    },
    edgeLabel: {
      show: true,
      formatter(params: unknown) {
        const payload = params as { data?: { name?: string } }
        return payload.data?.name ?? ''
      },
    },
    lineStyle: {
      width: 3,
      opacity: 0.9,
    },
  },
})

const option: EChartsOption = {
  color: ['#2563eb', '#1e3a8a', '#7c3aed', '#16a34a', '#d97706', '#e11d48'],
  baseOption: {
    backgroundColor: '#f7f9fc',
    title: {
      text: '心理学知识图谱课程时间轴',
      subtext: '按大学四年展示课程与知识点覆盖关系',
      left: 24,
      top: 18,
      textStyle: {
        color: '#111827',
        fontSize: 22,
        fontWeight: 700,
      },
      subtextStyle: {
        color: '#64748b',
      },
    },
    tooltip: {
      trigger: 'item',
      formatter(params: unknown) {
        const payload = params as TooltipPayload

        if (payload.dataType === 'edge') {
          return `${payload.data.source} -> ${payload.data.target}<br/>关系：${payload.data.name}`
        }

        return `${payload.data.name}<br/>类型：${payload.data.category}<br/>${payload.data.value ?? ''}`
      },
    },
    legend: {
      right: 24,
      top: 24,
      orient: 'vertical',
      data: categories.map((item) => item.name),
    },
    timeline: {
      axisType: 'category',
      autoPlay: false,
      bottom: 18,
      left: 44,
      right: 44,
      currentIndex: 0,
      data: snapshots.map((snapshot) => snapshot.year),
      label: {
        color: '#475569',
      },
      checkpointStyle: {
        color: '#2563eb',
        borderColor: '#1d4ed8',
      },
      controlStyle: {
        color: '#2563eb',
        borderColor: '#2563eb',
      },
    },
    series: [makeGraphSeries(snapshots[0])],
  },
  options: snapshots.map((snapshot) => ({
    title: {
      subtext: `${snapshot.year}：${snapshot.courses.length}门课程，覆盖${snapshot.coverageCount}个知识点`,
    },
    series: [makeGraphSeries(snapshot)],
  })),
}

function handleTimelineChanged(payload: unknown) {
  const timelinePayload = payload as { currentIndex?: number }

  if (typeof timelinePayload.currentIndex === 'number') {
    activeIndex.value = timelinePayload.currentIndex
  }
}

onMounted(() => {
  if (!chartElement.value) return

  chart.value = init(chartElement.value)
  chart.value.setOption(option)
  chart.value.on('timelinechanged', handleTimelineChanged)

  resizeObserver = new ResizeObserver(() => {
    chart.value?.resize()
  })
  resizeObserver.observe(chartElement.value)
})

onBeforeUnmount(() => {
  resizeObserver?.disconnect()
  chart.value?.off('timelinechanged', handleTimelineChanged)
  chart.value?.dispose()
})
</script>

<template>
  <main class="page">
    <section class="workspace">
      <div class="chart-panel">
        <div ref="chartElement" class="chart"></div>
      </div>

      <aside class="info-panel">
        <p class="eyebrow">{{ activeSnapshot.year }}</p>
        <h1>本科四年心理学知识图谱</h1>
        <p class="description">按大学四年展示课程、前置关系与覆盖知识点。</p>

        <div class="metric-grid">
          <div>
            <strong>{{ activeSnapshot.courses.length }}</strong>
            <span>本年课程</span>
          </div>
          <div>
            <strong>{{ activeSnapshot.coverageCount }}</strong>
            <span>覆盖节点</span>
          </div>
        </div>

        <div class="legend" aria-label="节点类型图例">
          <span v-for="item in legendItems" :key="item.name" :class="['legend-item', item.className]">
            {{ item.name }}
          </span>
        </div>

        <div class="relation-legend" aria-label="关系类型图例">
          <span v-for="item in relationLegendItems" :key="item.name" class="relation-legend-item">
            <span class="relation-line" :style="{ '--relation-color': item.color }"></span>
            {{ item.name }}
          </span>
        </div>

        <div class="section">
          <h2>课程</h2>
          <ul class="course-list">
            <li v-for="course in activeSnapshot.courses" :key="course.id">
              <span>{{ course.name }}</span>
              <small>{{ course.category }}｜{{ course.coveredCount }}节点</small>
            </li>
          </ul>
        </div>

        <div class="section">
          <h2>前置关系</h2>
          <ul class="link-list">
            <li v-for="course in activeSnapshot.courses.filter((item) => item.prerequisites.length)" :key="course.id">
              {{ course.prerequisites.join('、') }} -> {{ course.name }}
            </li>
          </ul>
        </div>
      </aside>
    </section>
  </main>
</template>

<style scoped>
:global(*) {
  box-sizing: border-box;
}

:global(body) {
  margin: 0;
  min-width: 320px;
  color: #17202a;
  background: #e8edf3;
  font-family:
    Inter, ui-sans-serif, system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
}

.page {
  min-height: 100vh;
  padding: 24px;
}

.workspace {
  display: grid;
  grid-template-columns: minmax(0, 1fr) 360px;
  gap: 18px;
  min-height: calc(100vh - 48px);
}

.chart-panel,
.info-panel {
  border: 1px solid #d7dee8;
  background: #f7f9fc;
  box-shadow: 0 16px 48px rgba(15, 23, 42, 0.08);
}

.chart-panel {
  min-height: 1120px;
  overflow: hidden;
}

.chart {
  width: 100%;
  height: 100%;
  min-height: 1120px;
}

.info-panel {
  display: flex;
  flex-direction: column;
  gap: 18px;
  padding: 24px;
  overflow: auto;
}

.eyebrow {
  margin: 0;
  color: #2563eb;
  font-size: 13px;
  font-weight: 700;
  letter-spacing: 0;
}

h1,
h2,
p {
  margin-top: 0;
}

h1 {
  margin-bottom: 10px;
  font-size: 28px;
  line-height: 1.18;
}

h2 {
  margin-bottom: 6px;
  color: #344054;
  font-size: 16px;
}

.description {
  margin-bottom: 0;
  color: #526070;
  line-height: 1.7;
}

.legend {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin-top: 16px;
}

.legend-item {
  border-radius: 999px;
  padding: 5px 10px;
  font-size: 12px;
  font-weight: 700;
  line-height: 1.2;
}

.relation-legend {
  display: grid;
  gap: 8px;
  margin-top: -4px;
}

.relation-legend-item {
  display: inline-flex;
  align-items: center;
  gap: 10px;
  color: #475467;
  font-size: 13px;
  font-weight: 650;
}

.relation-line {
  position: relative;
  width: 42px;
  height: 0;
  border-top: 3px solid var(--relation-color);
}

.relation-line::after {
  position: absolute;
  top: -6px;
  right: -1px;
  width: 0;
  height: 0;
  border-top: 5px solid transparent;
  border-bottom: 5px solid transparent;
  border-left: 8px solid var(--relation-color);
  content: '';
}

.metric-grid {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 10px;
}

.metric-grid div {
  border: 1px solid #dbe3ee;
  border-radius: 8px;
  padding: 12px;
  background: #ffffff;
}

.metric-grid strong {
  display: block;
  color: #111827;
  font-size: 24px;
  line-height: 1;
}

.metric-grid span {
  display: block;
  margin-top: 8px;
  color: #667085;
  font-size: 12px;
}

.section {
  border-top: 1px solid #dfe6ef;
  padding-top: 18px;
}

.legend-item.course {
  color: #1d4ed8;
  background: #dbeafe;
}

.legend-item.domain {
  color: #1e3a8a;
  background: #dbeafe;
}

.legend-item.construct {
  color: #6d28d9;
  background: #ede9fe;
}

.legend-item.method {
  color: #166534;
  background: #dcfce7;
}

.legend-item.theory {
  color: #7c2d12;
  background: #ffedd5;
}

.legend-item.application {
  color: #9f1239;
  background: #ffe4e6;
}

.course-list,
.link-list {
  display: grid;
  gap: 8px;
  margin: 0;
  padding: 0;
  list-style: none;
}

.course-list li {
  display: grid;
  gap: 4px;
  border: 1px solid #dbe3ee;
  border-radius: 8px;
  padding: 10px 12px;
  background: #ffffff;
}

.course-list span {
  min-width: 0;
  font-weight: 650;
}

.course-list small {
  color: #667085;
}

.link-list li {
  border-left: 3px solid #93c5fd;
  padding: 4px 0 4px 10px;
  color: #475467;
  line-height: 1.5;
}

@media (max-width: 980px) {
  .page {
    padding: 12px;
  }

  .workspace {
    grid-template-columns: 1fr;
    min-height: auto;
  }

  .chart-panel,
  .chart {
    min-height: 620px;
  }
}
</style>
